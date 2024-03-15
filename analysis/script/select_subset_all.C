#include <string>
#include <vector>

#include "ROOT/RDataFrame.hxx"
#include "RemollData.hpp"

bool cut(RemollHit hit){
    //auto enabled = std::vector<int>{28,48,66,80};
    //return utl::contains(enabled,hit.det) and hit.r > 5  and hit.k > 1 and utl::cut::epm(hit)  ;
    return (utl::cut::epm(hit) or utl::cut::photon(hit) ) and utl::cut::md_ring(hit,0) and hit.k > 1;
}


hit_list get_subset_on_det(hit_list hits, hit_cut cut){
    std::vector<RemollHit>  rev_hits(0);
    for(auto hit: hits) if(cut(hit)) rev_hits.push_back(hit);
    return rev_hits;
}


void select_subset_all(std::string inputfile, std::string outputfile,std::string extension=".root"){
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame d("T",inputfile);
    std::vector<hit_list> selected;
    d.Foreach([&](hit_list hl) -> void {
        hit_list hits_on_md = get_subset_on_det(hl,cut); // get subset on Moller Detector
        if (hits_on_md.size()  > 0) selected.push_back(hits_on_md);
    },{"hit"});

    RemollData RD(outputfile+extension,"T",RemIO::WRITE);
    RD.add_branch(selected,"hit",true); // true because we want to write each element as row.
}

