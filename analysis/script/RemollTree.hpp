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

    RemollTree()=delete;
    RemollTree(std::string);
    RemollTree(TTree* T);
    int loop_init();
    bool next();


};

RemollTree::RemollTree(TTree* tree):T(tree) {
    cur_hits = nullptr;
    cur_parts = nullptr;
    cur_rate = 0;
    cur_entry = 0;
};

int RemollTree::loop_init(){
    T->SetBranchAddress("hit",&cur_hits);
    T->SetBranchAddress("part",&cur_parts);
    T->SetBranchAddress("rate",&cur_rate);
    totentries = T->GetEntries();
    return totentries;
}

bool RemollTree::next(){
    if(cur_entry < totentries){
        T->GetEntry(cur_entry);
        ++cur_entry;
        return true;
    }
   return false;
}


