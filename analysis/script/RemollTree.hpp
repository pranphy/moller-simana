#include <iostream>
#include <string>
#include <fstream>

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "utils.hh"

struct RemollTree {
    TTree* T = nullptr;
    hit_list* cur_hits;
    part_list* cur_parts;
    double cur_rate;
    int cur_entry;
    int totentries;
    RemollHitPartRate hitpartrate;

    RemollTree()=delete;
    RemollTree(std::string,std::string="T");
    RemollTree(TTree* T);
    int loop_init();
    bool next();
private:
    void init_params();
};

void RemollTree::init_params(){
    cur_hits = nullptr;
    cur_parts = nullptr;
    cur_rate = 0;
    cur_entry = 0;
}

RemollTree::RemollTree(TTree* tree):T(tree) {
    init_params();
};

RemollTree::RemollTree(std::string onefile,std::string treename){
    TFile* ifile = new TFile(onefile.c_str(),"READ");
    T = (TTree*)ifile->Get(treename.c_str());
    init_params();
}

int RemollTree::loop_init(){
    T->SetBranchAddress("hit",&cur_hits);
    T->SetBranchAddress("part",&cur_parts);
    T->SetBranchAddress("rate",&cur_rate);
    totentries = T->GetEntries();
    cur_entry = 0;
    return totentries;
}

bool RemollTree::next(){
    if(cur_entry < totentries){
        T->GetEntry(cur_entry);
        ++cur_entry;
        hitpartrate = std::make_tuple(*cur_hits,*cur_parts,cur_rate);
        return true;
    }
   return false;
}


