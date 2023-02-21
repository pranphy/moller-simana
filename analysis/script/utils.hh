#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#include "TChain.h"
#include "TTree.h"
#include "TH2D.h"
#include "TLatex.h"

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

TChain* make_chain(std::string filelist,bool verbose=false){
    std::ifstream flist(filelist);
    std::string filename;
    TChain* TC = new TChain("T");
    while(std::getline(flist,filename)){
        if(verbose) std::cout<<"Adding "<<filename<<std::endl;
        TC->Add(filename.c_str());
    }
    return TC;
}


double fill_hist2d(TH2D* hist2d, pair_func get_xy, TTree* T, std::function<hit_list(hit_list)> lookup, std::function<bool(RemollHit)> cut, bool rateweight=false) {
    hit_list  *hits=0; double rate;
    T->SetBranchAddress("hit",&hits);
    T->SetBranchAddress("rate",&rate);
    int totentry = T->GetEntries();
    double calc_rate = 0;
    for(int entry = 0; entry <= totentry; ++entry) {
        T->GetEntry(entry);
        //if(rate > 1e10) continue; // seems to me that 1e11 is the bogus rate value.
        hit_list rev_hits = lookup(*hits);
        if(!rateweight) rate = 1;
        bool flag = false;
        for(RemollHit hit: rev_hits ) {
            auto xyp = get_xy(hit);
            double x = xyp.first; double y = xyp.second;
            if( cut(hit) ) {
                flag = true;
                calc_rate += rate;
                hist2d->Fill(x,y,rate);
            }
        }
        //if(flag) calc_rate += rate;
    }
    return calc_rate;
}

void show_text(std::string text, Double_t normX, Double_t normY, Double_t size = 0.04) {
  TLatex* label = new TLatex();
  label->SetNDC(kTRUE);
  label->SetTextAlign(22);
  label->SetTextSize(size);
  label->SetTextColor(kRed);
  label->DrawLatex(normX, normY, text.c_str());
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

} // utl::
