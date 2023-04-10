// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2023-04-08

#include <string>
#include <vector>

#include "RemollTree.hpp"
#include "RemollData.hpp"

bool energetic_epm(RemollHit hit){
    return hit.e > 1 and utl::epm_cut(hit);
}

bool edge_6a(RemollHit hit){
    return  (hit.det == 66 and hit.r > 60 and hit.r <= 70) and energetic_epm(hit);
}

bool edge_6b(RemollHit hit){
    return (hit.det == 72 and hit.r > 80 and hit.r <= 110) and energetic_epm(hit);
}

/* Given a track find the corresponding hit at 6a, return true if there is such a hit at 6a
   (and return such hit object by reference) otherwise return false; */
bool track_at_6a(int track, hit_list hits, RemollHit& hit6a){
    for(auto hit : hits) if(hit.trid == track and hit.det == 66 ) { hit6a = hit; return true; }
    return false;
}

hit_list lookup(hit_list hits){
    hit_list selected; std::vector<int> tracks; RemollHit hitat6a;
    for(auto hit : hits) if(edge_6a(hit)) { selected.push_back(hit); tracks.push_back(hit.trid); };
    for(auto hit : hits) if(edge_6b(hit) and !utl::contains(tracks,hit.trid) and track_at_6a( hit.trid, hits, hitat6a)) selected.push_back(hitat6a);
    return selected;
}

void select_around_6a_6b(std::string inputfile, std::string outputfile,std::string extension=".root"){
    RemollTree RT(inputfile,"T");
    std::vector<hit_list> selected;

    // as I said std::vector<hit_list> with just one element. Because we want each to act as a identifiable event.
    auto fill_selected = [&](RemollHit hit) {selected.push_back({hit}); };
    loop_tree(RT,fill_selected,lookup);

    RemollData RD(outputfile+extension,"T",RemIO::WRITE);
    RD.add_branch(selected,"hit",true);
}

