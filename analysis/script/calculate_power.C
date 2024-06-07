// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2023-03-08

#include <ROOT/RDataFrame.hxx>

#include "utils.hh"

double sum_edep(ROOT::RDataFrame& df, std::vector<int> detlist={2004}){
    double edep = 0.0;
    df.Foreach([&](hit_list hl){
        for(auto hit: hl){
            if(utl::contains(detlist,hit.det)){
                edep += hit.edep;
            }
        }
    },{"hit"});
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
    TChain* T = utl::make_chain(filelist,100,true);
    //ROOT::EnableImplicitMT();
    ROOT::RDataFrame rd(*T);

    std::vector<int> detlist = {96,95};

    double power = get_power(sum_edep(rd,detlist),970000.0);
    printf(" Power = %0.3fW \n",power);


    TCanvas* c1 = new TCanvas("c1","c1");

    TH2D* rzhist = new TH2D("rzhist","Energy depositions at  Two Bounce Sheild; z[mm]; r[mm]",300,700,3100,300,20,40);
    T->Draw("hit.r:hit.z>>rzhist","hit.edep*(hit.det == 95 || hit.det==96 )","colz");
    rzhist->SetStats(kFALSE); rzhist->Draw("colz");
    utl::show_text(Form("Power = %.2fW",power),0.4,0.8);
    c1->SaveAs("asset/image/power/two-bounce-energy_depositions2.png");

    TCanvas* c2 = new TCanvas("c2","c2");
    TH1D* zhist = new TH1D("zhist","Energy depositions at  Two Bounce Sheild; z[mm]; E[MeV]",300,700,3100);
    T->Draw("hit.z>>zhist","hit.edep*(hit.det == 95 || hit.det==96 )","colz");
    zhist->SetStats(kFALSE); zhist->Draw("hist");
    utl::show_text(Form("Power = %.2fW",power),0.4,0.8);
    c2->SaveAs("asset/image/power/two-bounce-energy_depositions2-1D.png");

}

