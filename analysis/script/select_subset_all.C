// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2022-05-28

#include "utils.hh"
#include "ROOT/RDataFrame.hxx"

bool cut(RemollHit hit){
    std::vector<int> dets{38,39,911,44,45,46,47,483,484};
    return  utl::contains(dets,hit.det) and hit.k > 1 and (utl::cut::epm(hit) or utl::cut::photon(hit) );
}

hit_list get_subset_on_det(hit_list& hits) {
    std::vector<RemollHit>  rev_hits(0);
    for(auto hit: hits) if(cut(hit)) rev_hits.push_back(hit);
    return rev_hits;
}

void select_subset_all(std::string inputfile, std::string outputfile,std::string extension=".root"){
    //ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("T",inputfile);
    std::vector<hit_list> selected;

    df.Foreach([&](hit_list hl)->void {
        hit_list hits_on_md = get_subset_on_det(hl); // get subset on Moller Detector
        if (hits_on_md.size()  > 0) selected.push_back(hits_on_md);
    },{"hit"});

    std::cout<<selected.size()<<" is records written"<<std::endl;
    if(selected.size() > 0){
        ROOT::RDataFrame out(selected.size());
        int i = 0; auto d2 = out.Define("hit", [&](){ return selected.at(i++); });
        d2.Snapshot("T",outputfile+extension);
    } else { std::cout<<"No records passed the cut"<<std::endl; }
}

