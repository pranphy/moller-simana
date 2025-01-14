#!/usr/bin/env bash
# -*- coding: utf-8 -*-
# vim: ai ts=4 sts=4 et sw=4 ft=sh

# author : Prakash [प्रकाश]
# date   : 2022-11-18 13:50


mydir="/w/halla-scshelf2102/moller12gev/pgautam"
work_dir="${mydir}/pf/simana/remoll/sim"
output="${work_dir}/output"

source_dir="${mydir}/pf/sft/remoll/develop-remoll"

map_directory="${mydir}/pf/sft/remoll/data/field_maps"

remoll="../build/al9stepkill/remoll" # relative to geometry
#remoll="../build/fresh-develop/remoll" # relative to geometry
generator="beam"

echo "source_dir is ${source_dir}"
nrun=1000
evt_per_run=100000
detb=(28 4740)
detf=()

sim_id="${generator}-diffvacuum-cfg03"

#/remoll/addfield ${map_directory}/subcoil_2_3_3mm_real_asymmetric.txt
#/remoll/addfield ${map_directory}/V2U.1a.50cm.parallel.real_asymmetric.txt
#/remoll/addfield ${map_directory}/V2U.1a.50cm.parallel.txt
#/remoll/addfield ${map_directory}/V2DSg.9.75cm.parallel.txt
#Forked from driftpipe-flange-change. Just decreased the bellows3 IR by 6.35 mm = 0.25 in.

comment="""
From develop branch extend the target vacuum pipe upto 1219 and then shrink bellows1 to start from there. 
Add a virtual plane at 864mm and 1219mm. Make the inside of the vacuum pipe smaller sensitive with virtual cylinder (4740).
The internal cylinder 4740 is now made longer by 100 mm. In cfg2 I forgot to push the geometry to ifarm.
"""


## @fn
function write_macro() {
    ## Defines a function to write a macro
    macro_filename=${1}
    no_of_events=${2:-10000}
    output_name=${3}
    seed=${4}

cat > ${macro_filename} << EOF
/remoll/geometry/setfile geometry/mollerMother.gdml
/remoll/parallel/setfile geometry/mollerParallel.gdml

/remoll/physlist/parallel/enable
/run/numberOfThreads 5
/run/initialize
/remoll/printgeometry true

/remoll/addfield ${map_directory}/subcoil_2_3_3mm_real_asymmetric.txt
/remoll/addfield ${map_directory}/V2U.1a.50cm.parallel.real_asymmetric.txt

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


/remoll/step/killebelow 1 # MeV
/remoll/step/killafterz 23000 # mm


/remoll/SD/disable_all

EOF
for detno in "${detb[@]}"
do
cat >> ${macro_filename} << EOF
/remoll/SD/enable ${detno}
/remoll/SD/detect lowenergyneutral ${detno}
/remoll/SD/detect secondaries ${detno}
/remoll/SD/detect boundaryhits ${detno}

EOF
done


for detno in "${detf[@]}"
do
cat >> ${macro_filename} << EOF
/remoll/SD/enable ${detno}
/remoll/SD/detect lowenergyneutral ${detno}
/remoll/SD/detect secondaries ${detno}

EOF
done

cat >> ${macro_filename} << EOF
/process/list
/remoll/seed $(date +%M%S)${seed}
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

#source /site/12gev_phys/softenv.sh 2.4
#source /w/halla-scshelf2102/moller12gev/pgautam/pr/st/opt/root-6.28.06-tst/bin/thisroot.sh
cat > ${batch_filename} << EOF
#!/usr/bin/env bash
#SBATCH --ntasks=1
#SBATCH --job-name=${sim_id}
#SBATCH --output=log/${sim_id}/log-${count}-%j.log
#SBATCH --error=log/${sim_id}/log-${count}-%j.log
#SBATCH --mem-per-cpu=1000
#SBATCH --partition=production
#SBATCH --account=halla
#SBATCH --time=2:55:00

pwd

source /w/halla-scshelf2102/moller12gev/pgautam/pf/sft/enableal9.sh

printf 'started: '
date

cd ${source_dir}

${cmd}

printf 'ended: '
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

cat > "${output}/${sim_id}/info.md" << EOF
Date: $(date +%Y-%m-%d-%H:%M:%S)
remoll Path: ${remoll}
Event Per Run: ${evt_per_run}
Active detectors:
    Boundary: ${detb[@]}
    Full: ${detf[@]}
${comment}
EOF

for ((i=1; i <= $nrun; i++))
do
    num=$(printf "%04d" ${i} )

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


