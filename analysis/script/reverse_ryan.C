#include <string>
#include "TCanvas.h"
#include "TChain.h"
#include "TH2D.h"

#include "utils.hh"
#include "RemollTree.hpp"
#include "RemollData.hpp"

/* reverse_lookup: Given a list of hits, select those hits with `hit.det == from`,
   for those events find the trackid and look up event in the list of hits again
   to find if that track also has a hit at `hit.det==to`. If so, return the list
   of such hits at `to`.
   Now accepts `ring` parameter which applies the radial cut for various numbered
   rings. The rings radii are defined for MOLLER detector, but  could be thought
   as any regular radial cut. HA HA
*/
hit_list reverse_lookup(hit_list hits,int from /*449*/) {
    std::vector<int> trids;
    std::vector<RemollHit>  rev_hits(0);
    for(auto hit: hits){
        if(hit.det == from){
            trids.push_back(hit.trid);
        }
    }
    for(auto hit: hits) {
        auto where = std::find(trids.begin(),trids.end(),hit.trid);
        if(where != trids.end()) rev_hits.push_back(hit);
    }
    return rev_hits;
}


void reverse_ryan(){
    //std::string filelist = "/w/halla-scshelf2102/moller12gev/pgautam/pf/simana/sim/output/no-lintel-20230206-155101/primary/filelist.txt";
    std::string filelist = "/w/halla-scshelf2102/moller12gev/ryanrich/ferrous/remoll/ryan_develop_background_collar1_IR_623.txt";
    RemollTree RT(utl::make_chain(filelist,0,true));
    std::vector<hit_list> selected;
    for(RT.loop_init(); RT.next();){
        hit_list all_hits_on_other = reverse_lookup(*RT.cur_hits,449);
        if (all_hits_on_other.size()  > 0) selected.push_back(all_hits_on_other);
    }
    RemollData RD("sim/output/ryan/selected-drift-pipe-end-449-to-all-IR_623-20230228-1621.root","T",RemIO::WRITE);
    RD.add_branch(selected,"hit",true); // true because we want to write each element as row.
}

