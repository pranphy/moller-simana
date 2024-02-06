// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2023-03-08

#include <string>

#include "utils.hh"
#include "RemollTree.hpp"

double hypot(double x, double y) { return sqrt(x*x + y*y); };

double sum_edep(RemollTree& RT,std::vector<int> detlist={2004}){
    double edep = 0.0;
    for(RT.loop_init(); RT.next();){
        for(auto hit: *RT.cur_hits){
            if(utl::contains(detlist,hit.det)){
                edep += hit.edep;
            }
        }
    }
    return edep*1.0e6; // in ev
}

double get_power(double edep,int n_sim = 99000){
    double I = 65e-6; // 65uA
    double e = 1.6e-19; // charge
    double electron_rate = I/e; // no of electron per unit time
    double edep_per_sim_e = (edep*e)/n_sim; // in joules e
    double power = edep_per_sim_e*electron_rate;
    return power;
}

void calculate_power(std::string filelist="sim/output/add-kryptonite-202303081405/primary/filelist.txt"){
    //std::string filelist = "sim/output/add-kryptonite-202303081405/primary/filelist.txt";
    //std::string filelist = "sim/output/beam-dmap-NOQTZ-4IR256AIR556BIR80-power-202304281451/primary/filelist.txt";

    TChain* T = utl::make_chain(filelist,100,true);
    RemollTree RT(T,{"hit"});

    std::vector<int> detlist = {2004};

    double power = get_power(sum_edep(RT,detlist),200000);
    printf(" Power = %0.3fW \n",power);


    TCanvas* c1 = new TCanvas("c1","c1");

    TH2D* rzhist = new TH2D("rzhist","Energy depositions at Collimator 4, 200k events; z[mm]; r[mm]",300,3210,3380,300,10,260);
    T->Draw("hit.r:hit.z>>rzhist","hit.edep*(hit.det == 2004 )","colz");
    rzhist->SetStats(kFALSE); rzhist->Draw("colz");
    utl::show_text(Form("Power = %.2fW",power),0.4,0.8);
    c1->SaveAs("asset/image/power/ir4-25-coll-4-rz-energy_depositions.png");

    auto xyhist = hst::xy(RT,msc::detid_cut(2004),{-300,300,-300,300},300,"Power at Collimator 4");

    xyhist->Draw("colz")



    TCanvas* c2 = new TCanvas("c2","c2");
    TH1D* ehist = new TH1D("ehist","Edep; E[MeV]; Count",100,0,4);
    T->Draw("hit.edep>>ehist","hit.det == 2006");
    ehist->Draw();
    utl::show_text(Form("Power = %.2fkW",power/1000.0),0.4,0.8);
    c2->SaveAs("asset/image/power/ir4-25-6a-edep_1dhist_n.png");
}

