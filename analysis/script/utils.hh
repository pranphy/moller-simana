#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <functional>


#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TH2D.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TArc.h"

typedef remollGenericDetectorHit_t RemollHit;
typedef std::vector<RemollHit> hit_list;
typedef std::function<bool(RemollHit)> hitfunc;
typedef std::function<bool(RemollHit)> hit_cut;

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

// C++ Helpers

template <typename T>
bool contains(std::vector<T>& container,const T& elem) {
    return container.end() != std::find(container.begin(),container.end(),elem);
}

template <typename T>
bool is_inside(const T& x, const T& y,std::vector<T> box){
    return (x > box[0] and x <= box[1] and y > box[2] and y <= box[3]); //box = [x0,x1, y0, y1]
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

double hypot(double x, double y) { return sqrt(x*x + y*y); };
double to_degree(double a) { return a*180/3.141592653589;}
double to_radian(double a) { return a*3.141592653589/180;}

// ROOT Stuffs

void save(TObject* Obj, std::string name, std::string directory="", std::string rootfile="asset/files/saved-root-objects.root"){
    auto F = std::make_unique<TFile>(rootfile.c_str(),"UPDATE");
    if(directory != ""){
        TDirectory* objdir = F->GetDirectory(directory.c_str());
        if(!objdir) objdir = F->mkdir(directory.c_str());
        F->cd(directory.c_str());
    }
    Obj->Write(name.c_str());
    F->Close();
}

template<typename T> void save(std::string treename, std::vector<T> data, std::string name, std::string directory="", std::string rootfile="asset/files/saved-root-data.root"){
    auto F = std::make_unique<TFile>(rootfile.c_str(),"UPDATE");
    if(directory != ""){
        TDirectory* objdir = F->GetDirectory(directory.c_str());
        if(!objdir) objdir = F->mkdir(directory.c_str());
        F->cd(directory.c_str());
    }

    TTree* tree;
    if (F->Get(treename.c_str()) == nullptr) tree = new TTree(treename.c_str(), "Data Tree");
    else tree = dynamic_cast<TTree*>(F->Get(treename.c_str()));

    T elem;
    tree->Branch(name.c_str(), &elem);
    F->cd();
    for(auto dt: data) {elem = dt; tree->Fill(); }
    F->Write();
    F->Close();
}

template<typename T>
void save(std::vector<T> data, std::string name, std::string directory="", std::string rootfile="asset/files/saved-root-objects.root"){
    save("T",data,name,directory,rootfile);
}


/// Graphics stuffs
void draw_line(std::vector<double> p0, std::vector<double> p1){
    TLine* line = new TLine(p0[0],p0[1], p1[0],p1[1]);
    line->SetLineColor(kBlue); line->SetLineColor(2); line->SetLineWidth(1);
    line->Draw();
}

void add_vline(double xval){
    TLine* line = new TLine(xval, gPad->GetUymin(), xval, gPad->GetUymax());
    line->SetLineColor(kRed);
    line->SetLineStyle(2);
    line->SetLineWidth(1);
    line->Draw();
}

void add_hline(double yval){
    TLine* line = new TLine( gPad->GetUxmin(), yval, gPad->GetUxmax(),yval);
    line->SetLineColor(kRed);
    line->SetLineStyle(2);
    line->SetLineWidth(1);
    line->Draw();
}

void draw_circle(float radius, float x = 0, float y = 0){
    TArc* circle = new TArc(x, y, radius);
    circle->SetLineColor(kRed); circle->SetLineStyle(2); circle->SetLineWidth(1);
    circle->SetFillStyle(4001);
    circle->Draw();
}


template<typename T>
void draw_polygon(std::vector<T>& x, std::vector<T>& y,int colour=kRed, int thickness=2,int style=2){
    TPolyLine* poly = new TPolyLine(x.size(),x.data(),y.data());
    poly->SetLineWidth(thickness); poly->SetLineColor(colour); poly->SetLineStyle(style); poly->Draw();
}

template<typename T>
void make_rectangle(std::vector<T>& box,int colour=kRed, int thickness=2,int style=2){
    std::vector<float> x={box[0],box[0],box[1],box[1],box[0]};
    std::vector<float> y={box[2],box[3],box[3],box[2],box[2],};
    draw_polygon(x, y, colour, thickness, style);
}

void show_text(std::string text, Double_t normx, Double_t normy, int colour=kRed, Double_t size = 0.04) {
  TLatex* label = new TLatex();
  label->SetNDC(kTRUE);
  label->SetTextAlign(22);
  label->SetTextSize(size);
  label->SetTextColor(colour);
  label->DrawLatex(normx, normy, text.c_str());
}

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
    return (hit.det == DETID::MD) && (hit.r > rmin+offset) && (hit.r <= rmax+offset);
}

/* (e) (p)lus/(m)inus cut*/
bool epm_cut(RemollHit hit){
    return hit.pid == PID::ELECTRON || hit.pid == PID::POSITRON;
}

bool photon_cut(RemollHit hit){ return hit.pid == PID::PHOTON; }


namespace cut{
    bool md_ring(RemollHit hit,int ring)  { return md_ring_cut(hit,ring); };
    bool E1(RemollHit hit) { return hit.e > 1; }
    bool epm(RemollHit hit) { return epm_cut(hit); }
    bool ring5_epm(RemollHit hit){ return epm_cut(hit) and md_ring_cut(hit,5); }
    bool ring5_epm_E1(RemollHit hit){ return ring5_epm(hit) && E1(hit); }
    bool photon(RemollHit hit) {return photon_cut(hit); }
    bool ring5_photon_E1(RemollHit hit) {return photon_cut(hit) and md_ring_cut(hit,5) and E1(hit); }
}

bool __pass(RemollHit) { return true; }

typedef std::function<double(RemollHit)> __remhit_attrib_d;
auto __attrib_trid = [](RemollHit hit)->double { return hit.trid; };

hit_list select_tracks(hit_list hits, std::function<bool(RemollHit)> cut_s, std::function<bool(RemollHit)> cut_d = __pass, __remhit_attrib_d param=__attrib_trid){
    std::vector<int> trids;
    std::vector<RemollHit>  rev_hits;
    for(auto hit: hits) if(cut_s(hit)) trids.push_back(param(hit));
    for(auto hit: hits) if(contains(trids,hit.trid) and cut_d(hit)) rev_hits.push_back(hit);
    return rev_hits;
}

hit_list select_mother_tracks(hit_list hits, std::function<bool(RemollHit)> cut_s, std::function<bool(RemollHit)> cut_d = __pass){
    __remhit_attrib_d param = [](RemollHit hit)->double { return hit.mtrid; };
    return select_tracks(hits,cut_s,cut_d,param);
}


template <typename T>
bool det_pid_cut(T obj,int det, int pid) {
    return obj.det == det && (obj.pid == pid || obj.pid == -pid);
}

// Remoll Data
TTree* get_tree(std::string filename,const std::string treename="T"){
    TFile* F = new TFile(filename.c_str());
    TTree* T = F->Get<TTree>(treename.c_str());
    return T;
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

// Moller Specific |> calculations
float moller_fraction(float count, float primary){
    float moller_per_beam = 1.0/3300.0; // we expect 1 moller per 3300 beam events.
    float moller_number = moller_per_beam * primary;
    return count/moller_number;
}

float moller_fraction(float count, float secondary,float skim, float primary){
    float countdash = count/(secondary/skim);
    return moller_fraction(countdash,primary);
}

} // utl::

