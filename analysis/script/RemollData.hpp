#include <iostream>
#include <string>
#include <fstream>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "utils.hh"


template<typename T>
bool identity(T) { return true; }

enum class RemIO{
    READ=true,
    WRITE=false,
};

class RemollData
{
    public:
        RemollData(std::string filename,std::string treename="T",RemIO p = RemIO::READ);
        ~RemollData()
        {
            switch(rw){ case RemIO::WRITE : remoll_file->cd(); /*remoll_tree->Fill();*/ remoll_tree->Write(); default: remoll_file->Close(); }
            delete remoll_file;
        }
        std::string filename;
        std::string treename;
        RemIO rw;
        TTree* remoll_tree;
        TFile* remoll_file;
        /* Open remoll root file and return number of events in var branch passing cut cut" */
        int get_event_count(std::string var, std::string cut);
        double push_hit_to(hit_list** hit, hitfunc passes=identity<RemollHit>);

        template <typename branchobj>
        std::vector<branchobj> get_values(std::string branchname, std::function<bool(branchobj)> passes=identity<branchobj>,unsigned count=0);

        template <typename branchobj>
        std::vector<branchobj> get_values_alt(std::string branchname, std::function<bool(branchobj)> passes,unsigned count=0);

        template<typename objtype>
        void add_branch(std::vector<objtype>& objvec,std::string branchname);
};

RemollData::RemollData(std::string filename,std::string treename,RemIO p)
    :filename(filename),treename(treename),rw(p)
{
    remoll_file = new TFile(filename.c_str(),"READ");
    switch(rw)
    {
        case RemIO::READ:
            remoll_file = new TFile(filename.c_str(),"READ");
            remoll_tree = (TTree*) remoll_file->Get(treename.c_str()); break;
        case RemIO::WRITE:
            remoll_file = new TFile(filename.c_str(),"RECREATE");
            remoll_tree = new TTree(treename.c_str(), "skim tree");
    }
}

template<typename objtype>
void RemollData::add_branch(std::vector<objtype>& objvec,std::string branchname)
{
    remoll_tree->Branch(branchname.c_str(), &objvec);
    remoll_tree->Fill(); // not really sure about this here instead of destructor.
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

template <typename branchobj>
std::vector<branchobj> RemollData::get_values(std::string branchname, std::function<bool(branchobj)> passes,unsigned count)
{
    std::vector<branchobj> passvec;
    TTreeReader reader("T",remoll_file);
    TTreeReaderValue<std::vector<branchobj>>  obj(reader,branchname.c_str());
    unsigned cnt = 0;
    while(reader.Next())
    {
        for(auto cur_obj: *obj)
            if (passes(cur_obj))
                passvec.push_back(cur_obj);
       if(count > 0 && ++cnt >= count)
         break;
    }
    return passvec;
}

template <typename branchobj>
std::vector<branchobj> RemollData::get_values_alt(std::string branchname, std::function<bool(branchobj)> passes,unsigned count)
{
    long total_entries = remoll_tree->GetEntries();
    std::vector<branchobj>* objvec=0;
    std::vector<branchobj> passvec;

    remoll_tree->SetBranchAddress(branchname.c_str(), &objvec);

    for (long i=0; i < total_entries;i++){
        remoll_tree->GetEntry(i);
        for(auto curobj: *objvec)
        {
           if (passes(curobj))
              passvec.push_back(curobj);
        }
        if (count > 0 && i > count)
          break;
    }
    return passvec;
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

