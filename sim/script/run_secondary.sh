#!/usr/bin/env bash
# -*- coding: utf-8 -*-
# vim: ai ts=4 sts=4 et sw=4 ft=sh

# author : Prakash [प्रकाश]
# date   : 2022-11-18 13:50


mydir="/w/halla-scshelf2102/moller12gev/pgautam"
work_dir="${mydir}/pf/simana/sim"
output="${work_dir}/output"
sourcename="add-kryptonite"
#sourcename="ryan-remoll"
source_dir="${mydir}/pf/sft/remoll/${sourcename}"

branch="lintel-update"
#branch="develop-ryan"
remoll="../build/${branch}/remoll" # relative to geometry
generator="beam"

echo "source_dir is ${source_dir}"
nrun=200
evt_per_run=10000
dets=(28 61 62 63 64 65 66 67 69 72 74 76 77 80 81 815 816 817)


primary_id="real-assymetric-202303101758"
sim_id="${primary_id}"
pors="secondary" # primary or secondary
skimmed_primary_d="${output}/${primary_id}/primary/skim/merged/skim-around-coll-6a-202303141809.root"

skimmed_primary=${1:-"${skimmed_primary_d}"}


function write_macro()
{
    macro_filename=${1}
    no_of_events=${2:-10000}
    output_name=${3}
    seed=${4}

cat > ${macro_filename} << EOF
/remoll/geometry/setfile geometry/mollerMother.gdml
/remoll/parallel/setfile geometry/mollerParallel.gdml

/remoll/physlist/parallel/enable

/run/initialize
/remoll/printgeometry true

/remoll/addfield map_directory/subcoil_2_3_3mm_real_asymmetric.txt
/remoll/addfield map_directory/V2U.1a.50cm.parallel.real_asymmetric.txt

/remoll/evgen/set external
/remoll/evgen/external/file ${skimmed_primary}
/remoll/evgen/external/detid 66
/remoll/evgen/external/detid 69
/remoll/evgen/external/zOffset 0.001


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

/remoll/seed ${seed}
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
#SBATCH --job-name=${pors}-${sim_id}
#SBATCH --output=${log_dir}/log-${count}-%j.log
#SBATCH --error=${log_dir}/log-${count}-%j.log
#SBATCH --mem-per-cpu=1000
#SBATCH --partition=production
#SBATCH --account=halla
#SBATCH --time=03:20:00
#SBATCH --exclude=farm19104,farm19105,farm19106,farm19107,farm1996,farm19101
#
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

macro_dir="${source_dir}/macros/${sim_id}/${pors}"
batch_dir="${work_dir}/jobs/${sim_id}/${pors}"
log_dir="${work_dir}/log/${sim_id}/${pors}"
output_dir="${output}/${sim_id}/${pors}"

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
    write_sbatch ${batch_path} ${sim_id}/${pors}/${macro_name} ${num}
    echo "sbatch ${batch_path}"
    echo ""
    sbatch ${batch_path}
    echo ""
    sleep 1
done

