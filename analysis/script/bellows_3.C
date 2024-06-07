// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2024-05-29

#include "misc_utils.hh"

std::string opfile="./asset/files/root/bellows3-cfg07-plots-am.root";

auto bellows_3_us_epm   = [](RemollHit hit){ return  msc::detid_cut(483)(hit) and att::r(hit) > 55 and utl::cut::epm(hit); };
auto bellows_3_ds_epm   = [](RemollHit hit){ return  msc::detid_cut(484)(hit) and att::r(hit) > 55 and utl::cut::epm(hit); };
auto lead_shield_us_epm = [](RemollHit hit){ return  msc::detid_cut(46)(hit) and  att::r(hit) > 55 and utl::cut::epm(hit); };

std::map<int,std::string> detname{
    {38,  "Coll2 US"},
    {39,  "Coll2 DS"},
    {911,  "Mid Plane"},
    {44,  "Coll4 US"},
    {45,  "Coll4 DS"},
    {46,  "PbWall US"},
    {47,  "PbWall DS"},
    {483,  "Bellows3 US"},
    {484,  "Bellows3 DS"},
};

auto corr_hist(int det1, int det2, hit_cut basecut=utl::cut::epm, std::string id="E(all)"){
    auto title = std::string(Form(" %s corr %s and %s ; Radial hit %s [mm]; Radial hit %s [mm]",id.c_str(),detname[det1].c_str(),detname[det2].c_str(), detname[det1].c_str(),detname[det2].c_str()));
    auto name = std::string(Form("%s-rr-%d-%d",id.c_str(),det1,det2));
    return hst::hist(name,msc::rr_correlate(det1,det2,basecut),title,{300,40,120,300,10,335});
}

// Only possible 1d and 2d histograms.
std::vector<hst::hist> histograms{
    hst::hist("hits-on-bellows3-us-epm",bellows_3_us_epm,{att::x,att::y},"Bellows3 US; x[mm]; y[mm]",{600,-600,600,600,-600,600}), 
    hst::hist("hits-theta-r",bellows_3_us_epm,{att::r,att::th},"Bellows3 US; r[mm]; theta[deg]",{300,10,400,300,0,30}), 
    hst::hist("hits-theta-e",bellows_3_us_epm,{att::e,att::th},"Bellows3 US; E[MeV]; theta[deg]",{500,2,2000,500,0,10}), 
    hst::hist("hits-r-e",bellows_3_us_epm,{att::r,att::e},"Bellows3 US; r[mm]; E[MeV]",{500,10,400,500,0,8000}), 
    hst::hist("hits-on-bellows3-ds-epm",bellows_3_ds_epm,{att::x,att::y},"Bellows3 DS; x[mm]; y[mm]",{600,-600,600,600,-600,600}), 
    hst::hist("hits-on-lead-us-epm",lead_shield_us_epm,{att::x,att::y},"Lead Shield US; x[mm]; y[mm]",{600,-600,600,600,-600,600}), 
    hst::hist("energy-on-bellows3",bellows_3_us_epm,att::e,"Bellows3 US; E[MeV]; Count",{600,0,2000}), 
    hst::hist("phi-dist-bellows3",bellows_3_us_epm,att::phi,"Bellows3 US; #phi[degree]; count",{360,-180,180}), 
    hst::hist("tracks-pb-b3",msc::track_selector(msc::detid_cut(46)),bellows_3_us_epm,{att::x,att::y},"XY at bellows3 that also hit Pb Shield; x[mm]; y[mm]",{600,-600,600,600,-600,600}),
    corr_hist(39,38), corr_hist(39,44), corr_hist(39,45), corr_hist(39,46), corr_hist(39,47),  corr_hist(39,483), corr_hist(39,484),
    corr_hist(39,484,msc::all({utl::cut::epm,msc::E_cut(0,1000)}), "E(0-1000)"),
    corr_hist(39,484,msc::all({utl::cut::epm,msc::E_cut(1000,2000)}), "E(1000-2000)"),
    corr_hist(39,484,msc::all({utl::cut::epm,msc::E_cut(2000,8000)}), "E(2000-8000)"),
    corr_hist(39,484,msc::all({utl::cut::epm,msc::E_cut(10500)}), "E(10500-)"),
};


void bellows_3(std::string filename=""){
    if(filename == "") 
        filename = "sim/output/beam-bellows3-cfg05/primary/skim/merged/hit-pb34-epm-photon.root";

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("T",filename);
    hst::fill(df,histograms);
    //histograms[1].draw("colz");
    hst::save(histograms,"bellows3-cfg07/plot",opfile);
    std::cout<<" Saved: "<<opfile<<std::endl;
}

