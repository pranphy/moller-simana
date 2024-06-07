#include "utils.hh"
#include "ROOT/RDataFrame.hxx"

bool cut(RemollHit hit){
    // md_ring_cut takes hit and ring number and returns true if the hit is in that ring.
    // epm(hit) takes a hit objects return true if its either electrom or positron e+-
    //return utl::md_ring_cut(hit,0) and hit.k > 1 and (utl::cut::epm(hit) or utl::cut::photon(hit) );
    std::vector<int> dets{46,483,484,485,486,28};
    //return  utl::contains(dets,hit.det) and hit.k > 1 and (utl::cut::epm(hit) or utl::cut::photon(hit) );
    return  utl::contains(dets,hit.det) and hit.k > 1 and (utl::cut::epm(hit) or utl::cut::photon(hit) );
}

hit_list get_subset_on_det(hit_list& hits) {
    std::vector<RemollHit>  rev_hits(0);
    for(auto hit: hits) if(cut(hit)) rev_hits.push_back(hit);
    return rev_hits;
}

void select_subset_MD(std::string inputfile, std::string outputfile,std::string extension=".root"){
    //ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("T",inputfile);
    std::vector<hit_list> selected;

    df.Foreach([&](hit_list hl)->void {
        hit_list hits_on_md = get_subset_on_det(hl); // get subset on Moller Detector
        if (hits_on_md.size()  > 0) selected.push_back(hits_on_md);
    },{"hit"});

    std::cout<<selected.size()<<" is records written"<<std::endl;
    if(selected.size() > 0){
        ROOT::RDataFrame out(selected.size()); // creates new dtaframe to store skimmed data
        int i = 0; auto d2 = out.Define("hit", [&](){ return selected.at(i++); }); // fills the df with the data
        d2.Snapshot("T",outputfile+extension); // writes to the disk
    } else { std::cout<<selected.size()<<" is zero LOL "<<std::endl; }
}

