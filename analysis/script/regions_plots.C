#include <string>
#include <vector>
#include <functional>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "remolltypes.hh"
#include "utils.hh"
#include "misc_util.hh"
#include "RemollTree.hpp"

int mdring = 0;
std::string imagedir = "asset/image/new-krypto/primary/charged";
auto canvas = new TCanvas("canvas");

hit_list look_atleast_one(hit_list hits, std::function<bool(RemollHit)> cut){
    hit_list empty(0);
    for(auto hit: hits) if(cut(hit)) return hits;
    return empty;
}

hit_list identity(hit_list hits) { return hits; }

bool energy_cut(RemollHit hit) { return hit.e > 1; }

bool electron_hitting_md(RemollHit hit){
    return utl::md_ring_cut(hit,mdring)  and utl::epm_cut(hit); 
}

bool vz_cut(RemollHit hit,float vzmin, float vzmax){ return hit.vz > vzmin and hit.vz <= vzmax; }


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
    auto lookup = [&](hit_list hits) { return utl::select_tracks(hits,cut); };
    auto fill_rz = [&](RemollHit hit) { rzhist->Fill(hit.z,utl::hypot(hit.x,hit.y)); };
    loop_tree(RT,fill_rz,lookup);
    rzhist->SetStats(kFALSE);
    rzhist->Draw("colz");
    canvas->SaveAs(Form("%s/intercepted-by-virtual-planes-tracks-hitting-ring-%d-from-vz-%.0f-%.0f%s.pdf",imagedir.c_str(),mdring,vzmin,vzmax,suffix.c_str()));
}


void track_r_at_det(RemollTree& RT, float vzmin, float vzmax, int det,std::vector<float> bins={100,0,100},std::string suffix=""){
    TH1D* rhist = new TH1D("rhist",Form("All tracks that that originate at vz(%.0f,%.0f], then intersect det %d, and go on to hit in Ring %d; r[mm]; count",vzmin,vzmax,det,mdring),bins[0],bins[1],bins[2]);
    auto fill_r = [&](RemollHit hit) { if(hit.det == det ) rhist->Fill(hit.r); };
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit) && vz_cut(hit,vzmin,vzmax); };
    auto lookup = [&](hit_list hits) { return utl::select_tracks(hits,cut); };
    loop_tree(RT,fill_r,lookup);
    rhist->Draw();
    canvas->SaveAs(Form("%s/hit-r-all-tracks-that-originate-at-vz-%.0f-%.0f-and-intersect-det-%d-and-hit-ring-%d%s.png",imagedir.c_str(),vzmin,vzmax,det,mdring,suffix.c_str()));
}

void track_xy_at_det(RemollTree& RT, float vzmin, float vzmax, int det,std::vector<float> bins={100,-100,100,100,-100,100},std::string suffix=""){
    TH2D* xyhist = new TH2D("xyhist",Form("All tracks that that originate at vz(%.0f,%.0f], then intersect det %d, and go on to hit in Ring %d; x[mm]; y[mm]",vzmin,vzmax,det,mdring),bins[0],bins[1],bins[2],bins[3],bins[4],bins[5]);
    auto fill_xy = [&](RemollHit hit) { if(hit.det == det ) xyhist->Fill(hit.x,hit.y); };
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit) && vz_cut(hit,vzmin,vzmax); };
    auto lookup = [&](hit_list hits) { return utl::select_tracks(hits,cut); };
    loop_tree(RT,fill_xy,lookup);
    xyhist->Draw("colz");
    canvas->SaveAs(Form("%s/all-tracks-that-originate-at-vz-%.0f-%.0f-and-intersect-det-%d-and-hit-ring-%d%s.pdf",imagedir.c_str(),vzmin,vzmax,det,mdring,suffix.c_str()));
}


void hit_vrvz_hist(RemollTree& RT, float vzmin, float vzmax,std::vector<float> bins={200,0,28000,200,0,2000},std::string suffix=""){
    TH2D* rzhist = new TH2D("rzhist",Form("Vertices of hits at Ring %d from vz(%.0f,%.0f]; vz[mm]; vr[mm]",mdring,vzmin,vzmax),bins[0],bins[1],bins[2],bins[3],bins[4],bins[5]);
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit) && vz_cut(hit,vzmin,vzmax); };
    auto lookup = [&](hit_list hits) { return utl::select_tracks(hits,cut); };
    auto fill_rz = [&](RemollHit hit) { if(electron_hitting_md(hit)) rzhist->Fill(hit.vz,utl::hypot(hit.vx,hit.vy)); };
    //auto fill_rz = [&](RemollHit hit) {  rzhist->Fill(hit.vz,utl::hypot(hit.vx,hit.vy)); };
    loop_tree(RT,fill_rz,lookup);
    rzhist->Draw("colz");
    canvas->SaveAs(Form("%s/vertices-of-hit-at-ring-%d-from-vz-%.0f-%.0f-range%s.pdf",imagedir.c_str(),mdring,vzmin,vzmax,suffix.c_str()));
}



void count_annulus(RemollTree& RT, int detid, float rmin, float rmax,std::vector<float> bins={100,-80,80,200,-80,80},std::string suffix=""){
    TH2D* xyhist = new TH2D("xyhist",Form("The tracks hitting Ring %d passing through r [%.2f,%.2f) of det %d; vz[mm]; vr[mm]",mdring,rmin,rmax,detid),bins[0],bins[1],bins[2],bins[3],bins[4],bins[5]);
    auto cut = [&](RemollHit hit)->bool { return energy_cut(hit) && electron_hitting_md(hit); };
    auto lookup = [&](hit_list hits) { return utl::select_tracks(hits,cut); };
    auto detannulus = [&](RemollHit hit,int detid)->bool { return hit.det == detid && hit.r > rmin && hit.r < rmax; };
    auto fill_xy = [&](RemollHit hit) { if(detannulus(hit,detid)) xyhist->Fill(hit.x,hit.y); };
    loop_tree(RT,fill_xy,lookup);
    xyhist->Draw("colz");
    canvas->SaveAs(Form("%s/rpartition-hit-at-ring-%d-from-r-%.0f-%.0f-range%s.pdf",imagedir.c_str(),mdring,rmin,rmax,suffix.c_str()));
}

void other_stuffs(RemollTree& RT){
    for(auto detid : {44, 48, 58, 66 , 72}) utl::add_vline(msc::detloc[detid]);
    std::vector<float> rpartition = {0,40,45,50,55,90};
    for(int ring: {0,5}){
        mdring = ring;
        for(size_t i = 1; i < rpartition.size(); ++i){
            count_annulus(RT,66,rpartition[i-1],rpartition[i]);
        }
    }
}
// Ring %d/ all Ring

void regions_plots(){
    std::string directory = "sim/output/real-assymetric-202303271207/primary/skim/merged/";
    std::string filename = "events-at-least-1-epm-hit-on-all-rings.root";
    std::string primary_skim = directory+filename;
    auto T = utl::get_tree(primary_skim);
    RemollTree RT(T,{"hit"});

    for(auto detid : {44, 48, 58, 66 , 72}){
        float zloc = msc::detloc[detid];
        for(int ring : {0,5}){
            mdring = ring;
            for(float maxz : {2000,22000 }){
                track_xy_at_det(RT,-10000,maxz,detid,{100,-150,150,100,-150,150});
                track_rz_hist(RT,-10000,maxz,{100,zloc-200,12000,100,0,200},Form("-at-det-%d",detid)); 
                track_r_at_det(RT,-10000,2000,66,{100,20,160},"-");
                utl::add_vline(53.314);
            }
        }
    }
}

void test_secondary(){
    std::string secondary = "sim/output/real-assymetric-202303271207/secondary/epm3/IR53IA1.03e-2/skim/merged/events-at-least-1-hit-md.root";
    auto T = utl::get_tree(secondary);
    RemollTree RT(T,{"hit"});

    mdring = 5;
    track_xy_at_det(RT,-1e6,1e6,77,{100,-300,300,100,-300,300});
    track_r_at_det(RT,-1e6,1e6,72);
}

