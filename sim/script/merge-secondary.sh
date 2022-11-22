#!/usr/bin/env bash
# -*- coding: utf-8 -*-
# vim: ai ts=4 sts=4 et sw=4 ft=sh
# author : Prakash [प्रकाश]
# date   : 2022-11-21 13:16

mydir="/w/halla-scshelf2102/moller12gev/pgautam"
work_dir="${mydir}/pf/simana/sim"
source_dir="${mydir}/pf/sft/remoll/remoll"
reroot_dir="${mydir}/pf/sft/remoll/build/helicoil"
reroot="${reroot_dir}/reroot"

#sim_id="helicoil-20221121-113016"
sim_id="first-20221118-191058"
secondary_dir="${work_dir}/output//${sim_id}/secondary/"
list_dir="${work_dir}/files/"
mkdir -p ${list_dir}

detno=911

list_name="${list_dir}/${sim_id}-secondaries-all-files.txt"
rm ${list_name}

ls -1 ${secondary_dir}/*.root > ${list_name}
echo ${list_name} written

output_filename="${secondary_dir}/${sim_id}-merged.root"

merge_script="${work_dir}/script/MergeTree.C"

#cd ${scriptdir}
cmd=${reroot}" -b -q '"${merge_script}'("'${list_name}'","'${output_filename}'")'"'"

echo ${cmd}

