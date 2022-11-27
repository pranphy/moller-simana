#include "RemollData.hpp"

double RemollData::push_hit_to(hit_list** p_prev_hits, hitfunc passes)
{
    long total_entries = remoll_tree->GetEntries();
    hit_list* cur_hits = 0;

    remoll_tree->SetBranchAddress("hit", &cur_hits);

    double passed_hit_count = 0;

    for (long i=0; i < total_entries;i++){
        remoll_tree->GetEntry(i);
        for(size_t j=0; j < cur_hits->size(); ++j){
           if (passes(cur_hits->at(j)))
           {
              ++passed_hit_count;
              (*p_prev_hits)->push_back(cur_hits->at(j));
           }
        }
    }
    return passed_hit_count;
}

