#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <functional>


#include "TChain.h"
#include "TTree.h"
#include "TH2D.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TPolyLine.h"

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
    const int ELECTRON =  11;
    const int POSITRON = -11;
    const int PHOTON   =  22;
    const int PROTON   =  2212;
}

namespace DETID {
    const int MD = 28;
}

namespace utl{

// Remoll/Moller Specific

bool md_ring_cut(RemollHit hit, int ring = 0){
    int rmin = 0; int rmax = 0; int offset = 10; // Recheck offset
    if(ring == 0){ rmin = 650; rmax = 1160; } // ring = 0 means no ring cut
    else if(ring == 1) {rmin = 650; rmax = 680;} // Table from  DocDB 896-v1
    else if(ring == 2) {rmin = 680; rmax = 740; }
    else if(ring == 3) {rmin = 740; rmax = 800; }
    else if(ring == 4) {rmin = 800; rmax = 920; }
    else if(ring == 5) {rmin = 920; rmax = 1060; }
    else if(ring == 6) {rmin = 1060; rmax = 1160; }
    return (hit.r > rmin+offset) && (hit.r <= rmax+offset);
}



template<typename T, int trid>
bool trid_cut(T obj) { return obj.trid == trid; }

template<typename T, int pid>
bool pid_cut(T obj){return obj.pid == pid; }


template <typename T>
bool det_pid_cut(T obj,int det, int pid) {
    return obj.det == det && (obj.pid == pid || obj.pid == -pid);
}

TChain* make_chain(std::string filelist,int nof = 0, bool verbose=false){
    std::ifstream flist(filelist);
    std::string filename;
    TChain* TC = new TChain("T");
    int cnt = 0;
    while(std::getline(flist,filename)){
        if(++cnt > nof && nof > 0)  break;
        if(filename.length()<2) continue;
        if(verbose) std::cout<<"Adding "<<filename<<std::endl;
        TC->Add(filename.c_str());
    }
    return TC;
}


template <typename T>
bool has_pid(std::vector<T> hits, int pid) {
    return hits.end() != std::find_if(hits.begin(),hits.end(),[&](RemollHit hit)->bool{ return hit.pid == pid;});
}


bool has_electron(hit_list hits) { return has_pid(hits,PID::ELECTRON) || has_pid(hits,PID::POSITRON); }
bool has_proton(hit_list hits) { return has_pid(hits,PID::PROTON) ; }


/// Graphics stuffs

void make_rectangle(TCanvas* canvas,std::vector<float>& box,int color=kRed, int thickness=2,int style=2){
    std::vector<double> x={box[0],box[0],box[1],box[1],box[0]};
    std::vector<double> y={box[2],box[3],box[3],box[2],box[2],};
    TPolyLine* poly = new TPolyLine(x.size(),x.data(),y.data());
    poly->SetLineWidth(thickness); poly->SetLineColor(color); poly->SetLineStyle(style); poly->Draw();
    canvas->Update();
}

void show_text(std::string text, Double_t normX, Double_t normY, int color=kRed, Double_t size = 0.04) {
  TLatex* label = new TLatex();
  label->SetNDC(kTRUE);
  label->SetTextAlign(22);
  label->SetTextSize(size);
  label->SetTextColor(color);
  label->DrawLatex(normX, normY, text.c_str());
}

// C++ Helpers

template <typename T>
bool contains(std::vector<T>& container,const T& elem) {
    return container.end() != std::find(container.begin(),container.end(),elem);
}

template <typename T>
void print(std::vector<T> container, std::string formatter="%8.1f, "){
    printf("[ ");
    std::for_each(container.begin(),container.end(),[&](T c){ printf(formatter.c_str(),c);});
    printf("] ");
}

template <typename T>
void println(std::vector<T> container, std::string formatter="%8.1f, "){
    print(container,formatter);
    printf("\n");
}

// General utilities

double to_degree(double a) { return a*180/3.141592653589;}

} // utl::

