#!/usr/bin/env bash
# -*- coding: utf-8 -*-
# vim: ai ts=4 sts=4 et sw=4 ft=sh

# author : Prakash [प्रकाश]
# date   : 2022-11-18 13:50


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

/remoll/addfield ${map_directory}/subcoil_2_3_3mm_real_asymmetric.txt
/remoll/addfield ${map_directory}/V2U.1a.50cm.parallel.real_asymmetric.txt

/remoll/evgen/set external
/remoll/evgen/external/file ${skimmed_primary}
/remoll/evgen/external/detid ${sdetid}
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
#SBATCH --time=00:20:00
#SBATCH --exclude=farm19104,farm19105,farm19106,farm19107,farm1996,farm19101
#
source /site/12gev_phys/softenv.sh 2.4

printf 'started: '
date

cd ${source_dir}

${cmd}

printf 'ended: '
date

EOF

}

# Start execution from here
function run_sim(){
    macro_dir="${source_dir}/macros/${sim_id}/${pors}"
    batch_dir="${work_dir}/jobs/${sim_id}/${pors}"
    log_dir="${work_dir}/log/${sim_id}/${pors}"
    output_dir="${output}/${sim_id}/${pors}"

    echo "Output dir ${output_dir}"
    echo "macro dir ${macro_dir}"

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
        write_macro ${macro_path} ${evt_per_run} ${output_name} ${i}
        write_sbatch ${batch_path} ${sim_id}/${pors}/${macro_name} ${num}
        echo "sbatch ${batch_path}"
        echo ""
        sbatch ${batch_path}
        echo ""
    done
}

function loop_angles(){
    primary_id="real-assymetric-202303271207"
    sim_id="${primary_id}"
    skimmed_primary="${mydir}/pf/simana/sim/output/real-assymetric-202303271207/primary/skim/merged/hits-around-6a-976-files.root"

    sdetid=66

    sims=("IR40IA1.03e-2" "IR45IA1.03e-2" "IR50IA1.03e-2" "IR53IA1.03e-2")
    for cur_id in ${sims[@]}
    do
        angid=${cur_id}
        source_dir="${mydir}/pf/sft/remoll/angle-6a/${angid}"
        pors="secondary/epm/${angid}" # primary or secondary

        echo "Running for ${angid} "

        run_sim 
    done
}

# Kind of like a initialization function as an entry point.
function main(){
    mydir="/w/halla-scshelf2102/moller12gev/pgautam"
    work_dir="${mydir}/pf/simana/sim"
    output="${work_dir}/output"
    map_directory="${mydir}/pf/sft/remoll/data/field_maps"

    local branch="lintel-update"
    remoll="../../build/${branch}/remoll" # relative to geometry

    nrun=200
    evt_per_run=10000
    dets=(28 44 45 48 49 58 60 61 62 63 64 65 66 67 69 72 74 76 77 80 81 815 816 817)
    loop_angles
}

main

