#pragma once

/*!
  A long comment of.
*/
#include <iostream>
#include <string>
#include <fstream>

#include "TFile.h"
#include "TTree.h"

#include "utils.hh"

static const std::vector<std::string> default_branches = {"hit","rate","part"};

/**
  @deprecated I have now discovered that we should be useing ROOTDataFrame.hxx instead of this method. Using
  this new ROOTDataFrame has a enormous speed advantage, partly because of easy parallelization and also it is
  better optimized by default.

  This class is used to loop through the branches of remoll data structure. This class consists pointers to 
  the hit, part and rate branches of the root file. There are more branches in the file but this only has the
  accessors for these three branches, because almost always we want these three branches. And the hit branch
  in particular.
  */
struct RemollTree {
    TTree* T = nullptr;
    //! This holds the pointer to the hit branch. Each event of hit branch is \ref hit_list.
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

/**
  A better name would bave been just init() but I accidentally left this name in a lot of scripts so this is
  still called loop_init().

  This method initializes the RemollTree object for loop.  For example it is used as
  ```{cpp}
  auto T = utl::get_tree("path/to/remoll/output.root");
  RemollTree RT(T,{"hit"});
  for(RT.loop_init(); RT.next();){
    //// statements
  } ```
*/
int RemollTree::loop_init(){
    if(utl::contains(branches,std::string("hit"))) T->SetBranchAddress("hit",&cur_hits);
    if(utl::contains(branches,std::string("part"))) T->SetBranchAddress("part",&cur_parts);
    if(utl::contains(branches,std::string("rate"))) T->SetBranchAddress("rate",&cur_rate);
    totentries = T->GetEntries();
    cur_entry = 0;
    return totentries;
}

/**
  Like \ref loop_init, this function is used for for loop. This basically reads next
  entry in the root tree and populates the reference to the objects ( one or more of hit, part and rate).
  \see loop_init
*/
bool RemollTree::next(){
    if(cur_entry < totentries){
        T->GetEntry(cur_entry++);
        return true;
    }
   return false;
}

//hit_list identity(hit_list hits) { return hits; }
typedef std::function<hit_list(hit_list&)> lookup_func;
lookup_func identity_lookup = [](hit_list& hits)->hit_list { return hits;};

/**
   Basically I ddidn't want to rewrite  this loop over and  over again. What I found was, I was looking up
   for each hits in an eventt. But if we only want the subset of the hit array for  the particular   event
   then we can use the lookup function. Lookup function takes a  hit array and returns the subset of those
   hits. User has the choice to use any cuts in lookup function. And for such subset loop through each hit
   and use the callback function to perform the task that we want. The order of callback and lookup functions
   is such that we can have lookup be identity function whenever we want to loop events for each hits.
*/
void loop_tree(RemollTree& RT, std::function<void(RemollHit)> callback, lookup_func lookup=identity_lookup){
    for(RT.loop_init(); RT.next();){
        hit_list looked_up = lookup(*RT.cur_hits);
        for(auto hit: looked_up){
            callback(hit);
        }
    }
}
