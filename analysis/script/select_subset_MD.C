#include <string>
#include <vector>

#include "RemollTree.hpp"
#include "RemollData.hpp"

bool cut(RemollHit hit){
    return utl::md_ring_cut(hit,0) && hit.e > 1 && (hit.pid == PID::ELECTRON || hit.pid == PID::POSITRON);
}


hit_list get_subset_on_det(hit_list hits, hit_cut cut){
    std::vector<RemollHit>  rev_hits(0);
    for(auto hit: hits) if(cut(hit)) rev_hits.push_back(hit);
    return rev_hits;
}


void select_subset_MD(std::string inputfile, std::string outputfile,std::string extension=".root"){
    RemollTree RT(inputfile,"T");
    std::vector<hit_list> selected;
    for(RT.loop_init(); RT.next();){
        hit_list hits_on_md = get_subset_on_det(*RT.cur_hits,cut); // get subset on Moller Detector
        if (hits_on_md.size()  > 0) selected.push_back(hits_on_md);
    }
    RemollData RD(outputfile+extension,"T",RemIO::WRITE);
    RD.add_branch(selected,"hit",true); // true because we want to write each element as row.
}

