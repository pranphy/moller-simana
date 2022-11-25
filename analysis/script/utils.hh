#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <functional>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"


typedef remollGenericDetectorHit_t RemollHit;
typedef std::vector<RemollHit> hit_list;
typedef std::function<bool(RemollHit)> hitfunc;



bool identity(RemollHit) { return true; }

class RemollData
{
    public:
        RemollData(std::string filename,std::string treename="T");
        ~RemollData()
        {
            remoll_file->Close();
            delete remoll_file;
        }
        std::string filename;
        std::string treename;
        TTree* remoll_tree;
        TFile* remoll_file;
        /* Open remoll root file and return number of events in var branch passing cut cut" */
        int get_event_count(std::string var, std::string cut);
        double push_hit_to(hit_list** hit, hitfunc passes=identity);
};

RemollData::RemollData(std::string filename,std::string treename)
    :filename(filename),treename(treename)
{
    remoll_file = new TFile(filename.c_str(),"READ");
    remoll_tree = (TTree*) remoll_file->Get(treename.c_str());
}

/* Open remoll root file and return number of events in var branch passing cut cut" */
int RemollData::get_event_count(std::string var, std::string cut)
{
    int count = remoll_tree->Draw(var.c_str(),cut.c_str(),"groff");
    return count;
}


void test_funct(std::string path)
{
    std::cout<<"Removed everything"<<path<<std::endl;
}



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
