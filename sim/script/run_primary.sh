#!/usr/bin/env bash
# -*- coding: utf-8 -*-
# vim: ai ts=4 sts=4 et sw=4 ft=sh

# author : Prakash [प्रकाश]
# date   : 2022-11-18 13:50


mydir="/w/halla-scshelf2102/moller12gev/pgautam"
work_dir="${mydir}/pf/simana/sim"
output="${work_dir}/output"

source_dir="${mydir}/pf/sft/remoll/col2/c2-ang-4.40/"

map_directory="${mydir}/pf/sft/remoll/data/field_maps"

remoll="../../build/fresh-develop/remoll" # relative to geometry
generator="beam"

echo "source_dir is ${source_dir}"
nrun=10000
evt_per_run=100000
dets=(28 38 39 44 45 48 49 58 60 61 62 63 64 65 66 67 69 72 74 76 77 80 81 120 121 122 123 124 125 126 127 128 129 130 131 817 820)
#dets=(28 66 67 69 72 74 820)

sim_id="${generator}-col2-cfg08"

function write_macro()
{
    macro_filename=${1}
    no_of_events=${2:-10000}
    output_name=${3}
    seed=${4}

#/remoll/addfield ${map_directory}/subcoil_2_3_3mm_real_asymmetric.txt
#/remoll/addfield ${map_directory}/V2U.1a.50cm.parallel.real_asymmetric.txt


cat > ${macro_filename} << EOF
/remoll/geometry/setfile geometry/mollerMother.gdml
/remoll/parallel/setfile geometry/mollerParallel.gdml

/remoll/physlist/parallel/enable
/run/numberOfThreads 5
/run/initialize
/remoll/printgeometry true

/remoll/addfield ${map_directory}/V2U.1a.50cm.parallel.txt
/remoll/addfield ${map_directory}/V2DSg.9.75cm.parallel.txt

/remoll/evgen/beam/origin 0 0 -7500 mm
/remoll/evgen/beam/rasx 5 mm
/remoll/evgen/beam/rasy 5 mm
/remoll/evgen/beam/corrx 0.065
/remoll/evgen/beam/corry 0.065
/remoll/evgen/beam/rasrefz -4.5 m


/remoll/beamene 11 GeV
/remoll/beamcurr 65 microampere

/remoll/evgen/set ${generator}
/remoll/evgen/beamPolarization +L
/remoll/field/equationtype 2
/remoll/field/steppertype 2

/remoll/field/print


/remoll/SD/disable_all
EOF

for detno in "${dets[@]}"
do
cat >> ${macro_filename} << EOF
/remoll/SD/enable ${detno}
/remoll/SD/detect lowenergyneutral ${detno}
/remoll/SD/detect secondaries ${detno}
/remoll/SD/detect boundaryhits ${detno}

EOF
done

cat >> ${macro_filename} << EOF
/process/list
/remoll/seed $(date +%s)${seed}
/remoll/filename ${output_name}

/run/beamOn ${no_of_events}

EOF
echo "${macro_filename} written with ${no_of_events} events"

}

function write_sbatch()
{
    batch_filename=${1}
    macro_name=${2}
    count=${3}

    cmd="${remoll} macros/${macro_name}"

cat > ${batch_filename} << EOF
#!/usr/bin/env bash
#SBATCH --ntasks=1
#SBATCH --job-name=${sim_id}
#SBATCH --output=log/${sim_id}/log-${count}-%j.log
#SBATCH --error=log/${sim_id}/log-${count}-%j.log
#SBATCH --mem-per-cpu=1000
#SBATCH --partition=production
#SBATCH --account=halla
#SBATCH --time=3:55:00

source /site/12gev_phys/softenv.sh 2.4

printf 'started'
date

cd ${source_dir}

${cmd}

printf 'ended'
date

EOF

}

# Start execution from here
macro_dir="${source_dir}/macros/${sim_id}"
batch_dir="${work_dir}/jobs/${sim_id}"
log_dir="${work_dir}/log/${sim_id}"
output_dir="${output}/${sim_id}/primary"

mkdir -p ${output_dir}
mkdir -p ${macro_dir}
mkdir -p ${batch_dir}
mkdir -p ${log_dir}

for ((i=1; i <= $nrun; i++))
do
    num=$(printf "%04d" ${i})

    output_name="${output_dir}/${sim_id}-${evt_per_run}-${num}.root"

    macro_name="${sim_id}-${num}.mac"
    macro_path="${macro_dir}/${macro_name}"
    batch_path="${batch_dir}/${sim_id}-${num}-sbatch.sh"
    write_macro ${macro_path} ${evt_per_run} ${output_name} ${i} # i is seed
    write_sbatch ${batch_path} ${sim_id}/${macro_name} ${num}
    echo "sbatch ${batch_path}"
    echo ""
    sbatch ${batch_path}
    echo ""
done

