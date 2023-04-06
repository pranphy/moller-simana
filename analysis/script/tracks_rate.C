// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2023-03-01

#include "RemollTree.hpp"


bool min_energy(RemollHit hit){ return hit.e > 1; } // 1 MeV

bool cut(RemollHit hit) {
    return utl::md_ring_cut(hit,5) && min_energy(hit) && utl::epm_cut(hit);
};

double get_rate_for_tracks(RemollTree& RT,int nof, bool doublecount=true){
    double lrate = 0;
    for(RT.loop_init(); RT.next();){
        bool atleast_one_hit = false;
        hit_list tracks = utl::select_tracks(*RT.cur_hits, cut);
        for(auto hit: tracks){
            if(hit.det == 72 and hit.r > 65){
                atleast_one_hit = true;
                if (doublecount)  lrate += RT.cur_rate;
            }
        }
        if(!doublecount && atleast_one_hit) lrate += RT.cur_rate;
    }
    lrate = (lrate/nof);
    return lrate;
}

void tracks_rate(std::string filelist){
    //TTree* T = utl::get_tree(" your file here.root");
    TChain* T = utl::make_chain(filelist);

    RemollTree RT(T,{"hit","rate"});
    //int nof = files->GetListOfFiles()->GetSize();
    int nof = 97;

    double lrate = get_rate_for_tracks(RT,nof);
    printf("%.3fGHz\n",lrate/1e9);
}

