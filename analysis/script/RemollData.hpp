#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <functional>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "remolltypes.hh"

typedef remollGenericDetectorHit_t RemollHit;
typedef std::vector<RemollHit> hit_list;
typedef std::function<bool(RemollHit)> hitfunc;



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
            switch(rw){ case RemIO::WRITE : remoll_file->cd(); remoll_tree->Fill(); remoll_tree->Write(); default: remoll_file->Close(); }
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
        std::vector<branchobj> get_values(std::string branchname, std::function<bool(branchobj)> passes);

        template <typename branchobj>
        std::vector<branchobj> get_values_alt(std::string branchname, std::function<bool(branchobj)> passes);

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
    remoll_tree->Branch(branchname.c_str(),&objvec);
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
std::vector<branchobj> RemollData::get_values(std::string branchname, std::function<bool(branchobj)> passes)
{
    std::vector<branchobj> passvec;
    TTreeReader reader("T",remoll_file);
    TTreeReaderValue<std::vector<branchobj>>  obj(reader,branchname.c_str());
    while(reader.Next())
    {
        for(auto cur_obj: *obj)
            if (passes(cur_obj))
                passvec.push_back(cur_obj);
    }
    return passvec;

}

template <typename branchobj>
std::vector<branchobj> RemollData::get_values_alt(std::string branchname, std::function<bool(branchobj)> passes)
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
    }
    return passvec;
}

