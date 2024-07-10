// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2024-05-29

#include "misc_utils.hh"

std::string opfile="./asset/files/root/collar2-cfg20-plots-ag.root";

std::map<int,std::string> detname{
    {80,  "Collar 2 IRing US"},
    {81,  "Collar 2 IRing DS"},
    {82,  "Collar 2 ORing US"},
    {83,  "Collar 2 ORing DS"},
};

auto det_cut = [](int detid){ return utl::all({utl::cut::det(detid),utl::cut::epm}); };

auto xy_hist(int detid){
    auto title = std::string(Form(" XY@ %s ; x[mm]; y[mm]",detname[detid].c_str()));
    auto name = std::string(Form("xy-%s",detname[detid].c_str()));
    return hst::hist{name,det_cut(detid),{att::x,att::y},title,{300,-1000,1000,300,-1000,1000}};
}

auto corr_hist(int det1, int det2, hit_cut basecut=utl::cut::epm, std::string id="E(all)"){
    auto title = std::string(Form(" %s corr %s and %s ; Radial hit %s [mm]; Radial hit %s [mm]",id.c_str(),detname[det1].c_str(),detname[det2].c_str(), detname[det1].c_str(),detname[det2].c_str()));
    auto name = std::string(Form("%s-rr-%d-%d",id.c_str(),det1,det2));
    return hst::hist(name,utl::rr_correlate(det1,det2,basecut),title,{300,40,350,300,10,335});
}

// Only possible 1d and 2d histograms.
std::vector<hst::hist> cfg20_hists(){
    return std::vector<hst::hist> {
        hst::hist{"vzvr-from-cal2-to-ring5",utl::cut::ring5_epm_E1,{att::vz,att::vr},"from col2 to ring5; vz[mm]; vr[mm]",{200,19202,19360,200,1008,1020}},
        hst::hist{"zr-at-col2",utl::all({utl::cut::det(80),utl::cut::epm}),{att::z,att::r},"from col2 to ring5; z[mm]; r[mm]",{200,19202,19360,200,1008,1020}},
        hst::hist{"r-at-col2",utl::all({utl::cut::det(80),utl::cut::r(400,1200),utl::cut::epm}),att::r,"R dist at col2; r[mm]; count",{200,50,2000}},
        hst::hist{"ang-at-col2",utl::all({utl::cut::det(80),utl::cut::r(1008,1014),utl::cut::epm}),att::th,"Angle at col2 @r \\in (1008-1014); angle[degree]; count",{200,0,6}},
        xy_hist(80), xy_hist(81), xy_hist(82), xy_hist(83),
        corr_hist(81,80), corr_hist(81,82), corr_hist(81,83),
    };
}


void collar_2(std::string filename,std::string id="collar2-cfg20"){
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("T",utl::readlines(filename));

    auto histograms = cfg20_hists();
    hst::fill(df,histograms);
    hst::save(histograms,id+"/plot",opfile);
    std::cout<<" Saved: "<<opfile<<std::endl;
}

