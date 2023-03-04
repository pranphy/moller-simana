#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "TFile.h"
#include "TTree.h"

#include "utils.hh"

static const std::vector<std::string> default_branches = {"hit","rate","part"};

struct RemollTree {
    TTree* T = nullptr;
    hit_list* cur_hits;
    part_list* cur_parts;
    double cur_rate;
    int cur_entry;
    int totentries;
    std::vector<std::string> branches;


    RemollTree()=delete;
    RemollTree(std::string onefile, const std::vector<std::string> = default_branches, const std::string="T");
    RemollTree(std::string onefile, const std::string="T", const std::vector<std::string> = default_branches);
    RemollTree(TTree* T, const std::vector<std::string> = default_branches);
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

RemollTree::RemollTree(TTree* tree, const std::vector<std::string> branches):T(tree),branches(branches) {
    init_params();
};

RemollTree::RemollTree(std::string onefile,std::vector<std::string>branches,std::string treename):branches(branches){
    TFile* ifile = new TFile(onefile.c_str(),"READ");
    T = (TTree*)ifile->Get(treename.c_str());
    init_params();
}

RemollTree::RemollTree(std::string onefile,std::string treename,const std::vector<std::string> branches)
    :RemollTree(onefile,branches,treename) {}

int RemollTree::loop_init(){
    if(utl::contains(branches,std::string("hit"))) T->SetBranchAddress("hit",&cur_hits);
    if(utl::contains(branches,std::string("part"))) T->SetBranchAddress("part",&cur_parts);
    if(utl::contains(branches,std::string("rate"))) T->SetBranchAddress("rate",&cur_rate);
    totentries = T->GetEntries();
    cur_entry = 0;
    return totentries;
}

bool RemollTree::next(){
    if(cur_entry < totentries){
        T->GetEntry(cur_entry++);
        return true;
    }
   return false;
}


