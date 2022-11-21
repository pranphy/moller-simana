#!/usr/bin/env bash
# -*- coding: utf-8 -*-
# vim: ai ts=4 sts=4 et sw=4 ft=sh

# author : Prakash [प्रकाश]
# date   : 2022-11-18 13:50


mydir="/w/halla-scshelf2102/moller12gev/pgautam"
work_dir="${mydir}/pf/simana/sim"
output="${work_dir}/output"
source_dir="${mydir}/pf/sft/remoll/remoll"


primary_id="first-20221118-191058"
sim_id="${primary_id}"
pors="secondary"

#sim_id="first-$(date +%Y%m%d-%H%M%S%N)"
#sim_id="helicoil-$(date +%Y%m%d-%H%M%S)"
#
detno=911
skimmed_primary="${output}/primary/${primary_id}/${primary_id}-skimmed-${detno}.root"

nrun=100
evt_per_run=10000


function write_macro()
{
    macro_filename=${1}
    no_of_events=${2:-10000}
    output_name=${3}
    seed=${4}

cat > ${macro_filename} << EOF

/remoll/setgeofile geometry/mollerMother.gdml
/remoll/parallel/setfile geometry/mollerParallel.gdml 
/remoll/physlist/parallel/enable 
/run/initialize
/remoll/printgeometry true
/remoll/addfield map_directory/V2DSg.9.75cm.parallel.txt
/remoll/addfield map_directory/V2U.1a.50cm.parallel.txt
/remoll/evgen/set external
/remoll/evgen/external/file ${skimmed_primary} 
/remoll/evgen/external/detno ${detno}
/remoll/evgen/external/zOffset 0.001

/remoll/SD/disable_all
/remoll/SD/enable ${detno}
/remoll/SD/detect lowenergyneutral ${detno}
/remoll/SD/detect secondaries ${detno}
/remoll/SD/detect boundaryhits ${detno}

/remoll/SD/enable 28
/remoll/SD/detect lowenergyneutral 28
/remoll/SD/detect secondaries 28
/remoll/SD/detect boundaryhits 28

/remoll/kryptonite/enable
/remoll/kryptonite/add VacuumKryptonite
/remoll/kryptonite/add Tungsten
/remoll/kryptonite/add Copper
/remoll/kryptonite/add Lead
/remoll/kryptonite/add CW95
/remoll/kryptonite/list
/process/list

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

    log_dir="${work_dir}/log/${sim_id}/${pors}"
    mkdir -p  ${log_dir}

    cmd="../build/helicoil/remoll macros/${macro_name}"

cat > ${batch_filename} << BEOF
#!//usr/bin/env bash
#SBATCH --ntasks=1
#SBATCH --job-name=${pors}-${sim_id}
#SBATCH --output=${logdir}/log-${count}-%j.log
#SBATCH --error=${logdir}/log-${count}-%j.log
#SBATCH --mem-per-cpu=5000
#SBATCH --partition=production
#SBATCH --account=halla
#SBATCH --time=02:20:00
#SBATCH --exclude=farm19104,farm19105,farm19106,farm19107,farm1996,farm19101
#
source /site/12gev_phys/softenv.sh 2.4

cd ${source_dir} 

${cmd}

BEOF
}

# Start execution from here

macro_dir="${source_dir}/macros/${sim_id}/${pors}"
batch_dir="${work_dir}/jobs/${sim_id}/${pors}"
output_dir="${output}/${sim_id}/${pors}"

mkdir -p ${output_dir}
mkdir -p ${macro_dir}
mkdir -p ${batch_dir}

for ((i=1; i < $nrun; i++))
do
    num=$(printf "%04d" ${i})

    output_name="${output_dir}/test-${sim_id}-${evt_per_run}-${num}.root"

    macro_name="${sim_id}-${num}.mac"
    macro_path="${macro_dir}/${macro_name}"
    batch_path="${batch_dir}/${sim_id}-${num}-sbatch.sh"
    write_macro ${macro_path} ${evt_per_run} ${output_name} ${i} # i is seed
    write_sbatch ${batch_path} ${sim_id}/${pors}/${macro_name} ${num}
    echo "sbatch ${batch_path}"
    echo ""
    #sbatch ${batch_path}
    echo "sbatch ${batch_path}"
    echo ""
    sleep 1
done

