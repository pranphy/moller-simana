#include <vector>
#include <string>

#include "RemollTree.hpp"
#include "RemollData.hpp"

//std::vector<float> vzpartition = {-1e5,-3700,8500,14500,22200, 35000,1e6}; // our original partition.
std::vector<float> vzpartition = {-1e5, -3700, 0, 2000, 5000, 8500, 14500, 22000, 35000, 1e6}; // our original partition.

bool energy_cut(RemollHit hit) { return hit.e > 1; } // only interested in events with E > 1MeV.

bool electron_hitting_all_rings_MD(RemollHit hit){
    return utl::md_ring_cut(hit,0)  && (hit.pid == PID::ELECTRON || hit.pid == PID::POSITRON);
}

hit_list get_tracks_hitting_MD_from(hit_list hits,float min_vz=-1e9, float max_vz=1e9) {
    std::vector<int> trids;
    std::vector<RemollHit>  rev_hits(0);
    for(auto hit: hits){
        if (!energy_cut(hit)) continue; // Doesn't pass energy cut? Don't bother.
        if(electron_hitting_all_rings_MD(hit) && hit.vz > min_vz && hit.vz <= max_vz){
            trids.push_back(hit.trid);
        }
    }
    for(auto hit: hits) {
        if (!energy_cut(hit)) continue; // TODO: This may be redundant here.
        if(utl::contains(trids,hit.trid)){
            rev_hits.push_back(hit);
        }
    }
    return rev_hits;
}

void select_tracks_hitting_MD(std::string inputfile, std::string outputfile, std::string extension=".root"){
    RemollTree RT(inputfile,"T");
    for(size_t i = 0; i < vzpartition.size() - 1; ++i){
        float vzmin = vzpartition[i]; float vzmax = vzpartition[i+1];
        std::vector<hit_list> selected;
        for(RT.loop_init(); RT.next();){
            hit_list  md_tracks = get_tracks_hitting_MD_from(*RT.cur_hits,vzpartition[i],vzpartition[i+1]);
            if (md_tracks.size()  > 0) selected.push_back(md_tracks);
        }
        auto suffix = std::string(Form("-vz-from_%07.0f_to_%07.0f-only",vzmin,vzmax));
        RemollData RD(outputfile+suffix+extension,"T",RemIO::WRITE);
        RD.add_branch(selected,"hit",true); // true because we want to write each elemeelet as row.
    }
}

