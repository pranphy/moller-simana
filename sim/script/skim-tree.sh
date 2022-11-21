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

#primary_id="helicoil-20221121-113016"
primary_id="first-20221118-191058"
primary_dir="${work_dir}/output/primary/${primary_id}"
list_dir="${work_dir}/files"
detno=911

list_name="${list_dir}/${primary_id}-all-files.txt"

#ls -1 ${primary_dir}/*.root >> ${list_name}
echo ${list_name} written
output_filename="${primary_dir}/${primary_id}-skimmed-${detno}.root"

skim_script="${work_dir}/script/SkimTree.C"
#skim_script="SkimTree.C"

#cd ${scriptdir}
cmd=${reroot}" -b -q '"${skim_script}'("'${list_name}'",'${detno}',"'${output_filename}'")'"'"
echo ${cmd}

