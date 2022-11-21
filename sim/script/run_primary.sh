#!/usr/bin/env bash
# -*- coding: utf-8 -*-
# vim: ai ts=4 sts=4 et sw=4 ft=sh

# author : Prakash [प्रकाश]
# date   : 2022-11-18 13:50


mydir="/w/halla-scshelf2102/moller12gev/pgautam"
work_dir="${mydir}/pf/simana/sim"
output="${work_dir}/output"
source_dir="${mydir}/pf/sft/remoll/remoll"

echo "source_dir is ${source_dir}"
nrun=1000
evt_per_run=100000

#sim_id="first-$(date +%Y%m%d-%H%M%S%N)"
sim_id="helicoil-$(date +%Y%m%d-%H%M%S)"


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

/control/execute macros/load_magnetic_fieldmaps.mac

/remoll/evgen/beam/origin 0 0 -7.5 m
/remoll/evgen/beam/rasx 5 mm
/remoll/evgen/beam/rasy 5 mm
/remoll/evgen/beam/corrx 0.065
/remoll/evgen/beam/corry 0.065
/remoll/evgen/beam/rasrefz -4.5 m


/remoll/beamene 11 GeV
/remoll/beamcurr 65 microampere

/remoll/evgen/set beam
/remoll/evgen/beamPolarization +L
/remoll/field/equationtype 2
/remoll/field/steppertype 2

/remoll/field/print


/remoll/SD/disable_all

/remoll/SD/enable 911
/remoll/SD/detect lowenergyneutral 911
/remoll/SD/detect secondaries 911
/remoll/SD/detect boundaryhits 911


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

    cmd="../build/helicoil/remoll macros/${macro_name}"

cat > ${batch_filename} << BEOF
#!//usr/bin/env bash
#SBATCH --ntasks=1
#SBATCH --job-name=${sim_id}
#SBATCH --output=log/${sim_id}/log-${count}-%j.log
#SBATCH --error=log/${sim_id}/log-${count}-%j.log
#SBATCH --mem-per-cpu=5000
#SBATCH --partition=production
#SBATCH --account=halla
#SBATCH --exclude=farm19104,farm19105,farm19106,farm19107,farm1996,farm19101
#
source /site/12gev_phys/softenv.sh 2.4

cd ${source_dir} 

${cmd}

BEOF
}

# Start execution from here

macro_dir="${source_dir}/macros/${sim_id}"
batch_dir="${work_dir}/jobs/${sim_id}"
log_dir="log/${sim_id}"

output_dir="${output}/primary/${sim_id}"
mkdir -p ${output_dir}

mkdir -p ${macro_dir}
mkdir -p ${batch_dir}
mkdir -p  ${log_dir}

for ((i=0; i < $nrun; i++))
do
    num=$(printf "%04d" ${i})

    output_name="${output_dir}/test-${sim_id}-${no_of_events}-${count}.root"

    macro_name="${sim_id}-${num}.mac"
    macro_path="${macro_dir}/${macro_name}"
    batch_path="${batch_dir}/${sim_id}-${num}-sbatch.sh"
    write_macro ${macro_path} ${evt_per_run} ${output_name} ${i} # i is seed
    write_sbatch ${batch_path} ${sim_id}/${macro_name} ${num}
    echo "sbatch ${batch_path}"
    echo ""
    sbatch ${batch_path}
    echo ""
    sleep 1
done

