#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#include "TChain.h"
#include "TTree.h"
#include "TH2D.h"

typedef remollGenericDetectorHit_t RemollHit;
typedef std::vector<RemollHit> hit_list;
typedef std::function<bool(RemollHit)> hitfunc;

typedef remollEventParticle_t RemollPart;
typedef std::vector<RemollPart> part_list;

typedef std::function<std::pair<double,double>(RemollHit)> pair_func;

typedef std::tuple<hit_list,part_list,double> RemollHitPartRate;

/*
template<typename Function, typename... Arguments>
auto curry(Function function, Arguments... args) {
    return [=](auto... rest) {
        return function(args..., rest...);
    }; // don't forget semicolumn
}
*/
namespace PID {
    const int ELECTRON = 11;
    const int PHOTON   = 22;
    const int PROTON   = 2212;
}

namespace DETID {
    const int MD = 28;
}


namespace utl{

template<typename T, int trid>
bool trid_cut(T obj) { return obj.trid == trid; }

template<typename T, int pid>
bool pid_cut(T obj){return obj.pid == pid; }


template <typename T>
bool det_pid_cut(T obj,int det, int pid) {
    return obj.det == det && (obj.pid == pid || obj.pid == -pid);
}

TChain* make_chain(std::string filelist){
    std::ifstream flist(filelist);
    std::string filename;
    TChain* TC = new TChain("T");
    while(std::getline(flist,filename)){
        std::cout<<"Adding "<<filename<<std::endl;
        TC->Add(filename.c_str());
    }
    return TC;
}


void fill_hist2d(TH2D* hist2d, pair_func get_xy, TTree* T, std::function<hit_list(hit_list)> lookup, std::function<bool(RemollHit)> cut, bool rateweight=false) {
    hit_list  *hits=0; double rate;
    T->SetBranchAddress("hit",&hits);
    T->SetBranchAddress("rate",&rate);
    int totentry = T->GetEntries();
    for(int entry = 0; entry <= totentry; ++entry) {
        T->GetEntry(entry);
        hit_list rev_hits = lookup(*hits);
        for(RemollHit hit: rev_hits ) {
            if(!rateweight) rate = 1;
            auto xyp = get_xy(hit);
            double x = xyp.first; double y = xyp.second;
            if( cut(hit) ) hist2d->Fill(x,y,rate);
        }
    }
}

void save_hist2d(TH2D* xyhist,std::string filenameop){
    //TH2F *hist = new TH2F("hist", "Example Histogram", 10, 0, 10, 10, 0, 10);
    //std::string filenameop = "test-hist2d-save.txt"
    ofstream of(filenameop,std::ios::out);
    of << "Test Here"<<std::endl;
    //
    TH2D* hist = xyhist;
    //
    int xbins = hist->GetNbinsX();
    int ybins = hist->GetNbinsX();
    //
    of<<"XBins : ";
    for (int i = 0; i <= xbins+1; ++i) { // +1 cuz
        float be = hist->GetXaxis()->GetBinLowEdge(i);
        std::cout <<be << ", ";
        of << be<<", ";
    }
    of<<std::endl<<"YBins : ";
    for (int i = 0; i <= ybins+1; ++i) { // +1 cuz
        float be = hist->GetYaxis()->GetBinLowEdge(i);
        std::cout <<be << ", ";
        of << be<<", ";
    }
    //
    of<<std::endl << "Bin values : "<<std::endl;
    for(int i = 0; i < xbins; ++i){
        for(int j = 0; j < ybins; ++j) {
            double val = hist->GetBinContent(i,j);
            of << val << ", ";
        }
        of<<std::endl;
    }
}


//void loop_content(TTree* T, TH2D* anghist, std::function<bool(RemollHit)> cut){
//    part_list* parts=0; hit_list  *hits=0; double rate;
//    T->SetBranchAddress("hit",&hits);
//    T->SetBranchAddress("part",&parts);
//    T->SetBranchAddress("rate",&rate);
//    int totentry = T->GetEntries();
//    for(int entry = 0; entry < totentry; ++entry) {
//        T->GetEntry(entry);
//        //bool passes = cut(*hits,*parts,rate);
//        if (true) {
//            for(auto hit: *hits){
//                if(cut(hit)) {
//                    double ang = atan(hit.py/hit.pz);
//                    double zz = hit.z;
//                    anghist->Fill(ang,zz,rate);
//                }
//            }
//        }
//    }
//}
}
