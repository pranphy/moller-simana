#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include <string>
#include <map>

#include "utils.hh"
#include "misc_util.hh"
#include "RemollTree.hpp"


std::string directory = "sim/output/real-assymetric-202303271207/primary/skim/merged/";
std::string filename = "events-at-least-1-epm-hit-on-all-rings.root";
std::string primary_skim = directory+filename;

auto T = utl::get_tree(primary_skim);
RemollTree RT(T,{"hit"});
int mdring = 0;

hit_list tracks_passing(hit_list hits, std::function<bool(RemollHit)> cut){
    std::vector<int> trids;
    std::vector<RemollHit>  rev_hits(0);
    for(auto hit: hits){
        if(cut(hit)){
            trids.push_back(hit.trid);
        }
    }
    for(auto hit: hits) {
        if(utl::contains(trids,hit.trid) ){
            rev_hits.push_back(hit);
        }
    }
    return rev_hits;
}

hit_list look_atleast_one(hit_list hits, std::function<bool(RemollHit)> cut){
    hit_list empty(0);
    for(auto hit: hits) if(cut(hit)) return hits;
    return empty;
}

hit_list identity(hit_list hits) { return hits; }

bool energy_cut(RemollHit hit) { return hit.e > 1; }

bool electron_hitting_md(RemollHit hit){
    return utl::md_ring_cut(hit,mdring)  && (hit.pid == PID::ELECTRON || hit.pid == PID::POSITRON);
}

bool vz_cut(RemollHit hit,float vzmin, float vzmax){ return hit.vz > vzmin and hit.vz <= vzmax; }

std::string imagedir = "asset/image/new-krypto/primary/charged";
auto canvas = new TCanvas("canvas");

void energy_hist(TTree* T, float vzmin, float vzmax, float emax=1230,float emin=1,std::string suffix=""){
    std::string ring5 = "hit.det == 28 && hit.r > 930 && hit.r <= 1070";
    std::string ep1 = "hit.e >1 && (hit.pid == 11 || hit.pid == -11) ";
    TH1D* ehist = new TH1D("ehist",Form("Energy for all tracks that hit Ring %d originating in vz(%.0f,%.0f]; E[MeV]; Count",mdring,vzmin,vzmax),100,emin,emax);
    T->Draw("hit.e>>ehist", Form("%s &&  %s && hit.vz > %.0f && hit.vz < %.0f",ring5.c_str(),ep1.c_str(),vzmin,vzmax));
    ehist->Draw();
    canvas->SaveAs(Form("%s/energy-for-all-tracks-that-hit-ring-%d-from-vz-%.0f-%.0f%s.pdf",imagedir.c_str(),mdring,vzmin,vzmax,suffix.c_str()));
}

void track_rz_hist(RemollTree& RT, float vzmin, float vzmax,std::vector<float> bins={200,0,28000,200,0,2000},std::string suffix=""){
    TH2D* rzhist = new TH2D("rzhist",Form("Intercepted by virtual planes, tracks hitting Ring %d from vz(%.0f,%.0f]; z[mm]; r[mm]",mdring,vzmin,vzmax),bins[0],bins[1],bins[2],bins[3],bins[4],bins[5]);
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit) && vz_cut(hit,vzmin,vzmax); };
    auto lookup = [&](hit_list hits) { return tracks_passing(hits,cut); };
    auto fill_rz = [&](RemollHit hit) { rzhist->Fill(hit.z,utl::hypot(hit.x,hit.y)); };
    loop_tree(RT,fill_rz,lookup);
    rzhist->SetStats(kFALSE);
    rzhist->Draw("colz");
    canvas->SaveAs(Form("%s/intercepted-by-virtual-planes-tracks-hitting-ring-%d-from-vz-%.0f-%.0f%s.pdf",imagedir.c_str(),mdring,vzmin,vzmax,suffix.c_str()));
}


void hit_xy_at_det(RemollTree& RT, float vzmin, float vzmax, int det,std::vector<float> bins={100,-100,100,100,-100,100},std::string suffix=""){
    TH2D* xyhist = new TH2D("xyhist",Form("All hits on det %d  with at least 1 hit in Ring %d from vz(%.0f,%.0f]; x[mm]; y[mm]",det,mdring,vzmin,vzmax),bins[0],bins[1],bins[2],bins[3],bins[4],bins[5]);
    auto fill_xy = [&](RemollHit hit) { if(hit.det == det && utl::hypot(hit.x,hit.y) > 10 ) xyhist->Fill(hit.x,hit.y); };
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit) && vz_cut(hit,vzmin,vzmax); };
    auto lookup = [&](hit_list hits) { return look_atleast_one(hits,cut); };
    loop_tree(RT,fill_xy,lookup);
    xyhist->Draw("colz");
    canvas->SaveAs(Form("%s/all-hits-on-det-%d-with-at-least-1-hit-on-ring-%d-vz-%.0f-%.0f%s.pdf",imagedir.c_str(),det,mdring,vzmin,vzmax,suffix.c_str()));
}

void track_xy_at_det(RemollTree& RT, float vzmin, float vzmax, int det,std::vector<float> bins={100,-100,100,100,-100,100},std::string suffix=""){
    TH2D* xyhist = new TH2D("xyhist",Form("All tracks that that originate at vz(%.0f,%.0f], then intersect det %d, and go on to hit in Ring %d; x[mm]; y[mm]",vzmin,vzmax,det,mdring),bins[0],bins[1],bins[2],bins[3],bins[4],bins[5]);
    auto fill_xy = [&](RemollHit hit) { if(hit.det == det ) xyhist->Fill(hit.x,hit.y); };
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit) && vz_cut(hit,vzmin,vzmax); };
    auto lookup = [&](hit_list hits) { return tracks_passing(hits,cut); };
    loop_tree(RT,fill_xy,lookup);
    xyhist->Draw("colz");
    canvas->SaveAs(Form("%s/all-tracks-that-originate-at-vz-%.0f-%.0f-and-intersect-det-%d-and-hit-ring-%d%s.pdf",imagedir.c_str(),vzmin,vzmax,det,mdring,suffix.c_str()));
}


void hit_vrvz_hist(RemollTree& RT, float vzmin, float vzmax,std::vector<float> bins={200,0,28000,200,0,2000},std::string suffix=""){
    TH2D* rzhist = new TH2D("rzhist",Form("Vertices of hits at Ring %d from vz(%.0f,%.0f]; vz[mm]; vr[mm]",mdring,vzmin,vzmax),bins[0],bins[1],bins[2],bins[3],bins[4],bins[5]);
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit) && vz_cut(hit,vzmin,vzmax); };
    auto lookup = [&](hit_list hits) { return tracks_passing(hits,cut); };
    auto fill_rz = [&](RemollHit hit) { if(electron_hitting_md(hit)) rzhist->Fill(hit.vz,utl::hypot(hit.vx,hit.vy)); };
    //auto fill_rz = [&](RemollHit hit) {  rzhist->Fill(hit.vz,utl::hypot(hit.vx,hit.vy)); };
    loop_tree(RT,fill_rz,lookup);
    rzhist->Draw("colz");
    canvas->SaveAs(Form("%s/vertices-of-hit-at-ring-%d-from-vz-%.0f-%.0f-range%s.pdf",imagedir.c_str(),mdring,vzmin,vzmax,suffix.c_str()));
}


// Ring %d/ all Ring
// All vz/ vz < 2000
/*
const std::map<int,float> zloc{{44,3224.05}, {48,4707.0}}

track_rz_hist(RT,0,2000,{100,2000,14000,100,0,200}); 
track_rz_hist(RT,-1e5,1e6,{100,2000,14000,100,0,200}); 

track_rz_hist(RT,-10000,22000,{100,msc::detloc[44]-200,msc::detloc[72]+200,100,0,200},"-at-all-det-"); 
*/

void neo_regions_plots(){
    for(auto detid : {44, 48, 58, 66 , 72}){
        float zloc = msc::detloc[detid];
        for(int ring : {0,5}){
            mdring = ring;
            for(float maxz : {2000,22000 }){
                track_xy_at_det(RT,-10000,maxz,detid,{100,-150,150,100,-150,150});
                track_rz_hist(RT,-10000,maxz,{100,zloc-200,12000,100,0,200},Form("-at-det-%d",detid)); 
            }
        }
    }
}

mdring = 5
track_rz_hist(RT,-10000,22000,{100,2000,13000,100,0,200})

for(auto detid : {44, 48, 58, 66 , 72}) utl::add_vline(msc::detloc[detid]);


T->Draw("hit.vz")
hit_vrvz_hist(RT,5000,14500,{100,5000,13000,100,0,200})


void energy_at_dets(RemollTree& RT, float vzmin, float vzmax, int detid , std::vector<float> bins={200,0,28000,200,0,2000},std::string suffix=""){
    TH1D* ehist = new TH1D("ehist",Form("Intercepted by , tracks hitting %d Ring %d from vz(%.0f,%.0f]; z[mm]; r[mm]",detid,mdring,vzmin,vzmax),100,0,1200);
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit) && vz_cut(hit,vzmin,vzmax); };
    auto lookup = [&](hit_list hits) { return tracks_passing(hits,cut); };
    auto fill_rz = [&](RemollHit hit) { if(hit.det == detid) ehist->Fill(hit.e); };
    loop_tree(RT,fill_rz,lookup);
    ehist->Draw();
    //ehist->SetStats(kFALSE);
    //rzhist->Draw("colz");
    //canvas->SaveAs(Form("%s/intercepted-by-virtual-planes-tracks-hitting-ring-%d-from-vz-%.0f-%.0f%s.pdf",imagedir.c_str(),mdring,vzmin,vzmax,suffix.c_str()));
}

mdring = 5

energy_at_dets(RT,-1e9,1e9,66)
