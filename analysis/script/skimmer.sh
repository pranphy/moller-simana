#!/usr/bin/env bash
# -*- coding: utf-8 -*-
# vim: ai ts=4 sts=4 et sw=4 ft=sh
# author : Prakash [प्रकाश]
# date   : 2023-03-22 11:26

workdir="/w/halla-scshelf2102/moller12gev/pgautam/pf/simana"

#sims=(01IA6.98e-3 04IA1.03e-2 01IA3.81e-2 NO6A)
#sims=(IA6.98e-3 IA1.03e-2 IA3.81e-2 NO6A)
#sims=("IR40IA1.03e-2" "IR45IA1.03e-2" "IR50IA1.03e-2" "IR53IA1.03e-2")
#sims=("6AIR656BIR90IA2.62e-2" "6AIR656BIR95IA2.62e-2" "6AIR656BIR100IA2.62e-2")
#sims=("NOQTZ-4IR256AIR606BIR90IA2.62e-2") # For 818 secondary
#sims=("beam-NOQTZ-4IR256AIR556BIR80-202304211410")

#sims=("sec-0.40-1.00-4IR256AIR55.316BIR80.00" "sec-0.50-1.00-4IR256AIR55.316BIR80.00" "sec-0.60-1.00-4IR256AIR55.316BIR80.00" "sec-0.80-1.00-4IR256AIR55.316BIR80.00")
#sims=("sec-0.60-0.70-4IR256AIR55.316BIR80.00" "sec-0.60-0.85-4IR256AIR55.316BIR80.00" "sec-0.60-1.00-4IR256AIR55.316BIR80.00")

#sims=("sec-0.90-1.50-4IR256AIR55.316BIR80.00" "sec-1.00-1.50-4IR256AIR55.316BIR80.00" "sec-1.50-1.50-4IR256AIR55.316BIR80.00" "sec-2.18-1.50-4IR256AIR55.316BIR80.00") # sixa
#sims=("sec-0.90-1.50-4IR256AIR55.316BIR80.00" "sec-0.90-2.00-4IR256AIR55.316BIR80.00")
#sims=( "secnb-0.40-1.00-4IR256AIR55.316BIR80.00" "secnb-0.50-1.00-4IR256AIR55.316BIR80.00" "secnb-0.60-1.00-4IR256AIR55.316BIR80.00" "secnb-0.80-1.00-4IR256AIR55.316BIR80.00" "secnb-0.90-1.00-4IR256AIR55.316BIR80.00" "secnb-1.00-1.00-4IR256AIR55.316BIR80.00" "secnb-1.50-1.00-4IR256AIR55.316BIR80.00" "secnb-2.18-1.00-4IR256AIR55.316BIR80.00"  )
#sims=("c2-ang-3.40" "c2-ang-3.44" "c2-ang-3.50" "c2-ang-4.00" "c2-ang-4.50" "c2-ang-5.00")
sims=( "c2-ang-4.10" "c2-ang-4.20" "c2-ang-4.30" "c2-ang-4.40")

for id in ${sims[@]}
do
    #curdir="${workdir}/sim/output/real-assymetric-202303271207/secondary/ir-change-correct/${id}"
    #primary_id="beam-dmap-bp-4IR256AIR556BIR80-202305021158"
    primary_id="beam-col2-cfg06"
    curdir="${workdir}/sim/output/${primary_id}/secondary/col2-cfg06-sec01/${id}"
    cd ${curdir}
    rm filelist.txt
    for file in $(ls | grep root)
    do
        readlink -f ${file}
    done | sort > filelist.txt
    mkdir -p ${curdir}/skim/merged
    cd -

    ${workdir}/analysis/script/pbatch "${workdir}/analysis/script/events_at_least_1_hit.C"\
        "${curdir}/filelist.txt "\
        "${curdir}/skim/events-at-least-1-hit-md"\
        "skim-${id}-202304211357"
done
