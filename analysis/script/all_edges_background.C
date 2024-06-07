// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2023-07-21

#include "utils.hh"
#include "ROOT/RDataFrame.hxx"

static const std::string example_file ="/mnt/stg/data/remoll/sim/output/beam-col2-cfg08/primary/skim/merged/events-at-least-1-hit-md.root";
static const std::string opfile_ex = "asset/files/bill_info_43_df_test.root";
static const std::string opid = "col2-cfg08";

typedef std::vector<float> Box;
typedef std::map<std::string, Box> Boxes;
typedef std::map<std::string,TH2D*> Hist2Ds;

const static Boxes components{ // for each components {vz0, vz1, r0, r1}
    {"All",                     {-10000,35000.0,0.0,3000.0}},
    {"All Tight",               {-3500,25000.0,0.0,1500.0}},
    {"Collimator 1",            {100.0,600.0,10.0,50.0}},
    {"Collimator 2",            {625.0, 849,5,120}},
    {"Two bounce shield",       {924, 3199.9, 20,65}}, // For real.
    {"Collimator 4",            {3200, 3350, 0,195}},
    {"Bellows 3",               {4250, 4700, 0,420}},
    {"Vaccum Enclosure US End", {4600,4900, 310, 350}}, // Ryan was looking at assymetry here.
    {"Photon Scraper",          {4701.00,4819.00,20.0,155.0}},
    {"Belly Plates 1",          {4900, 5942,32,57}},
    {"Belly Plates 2",          {5945, 7000,32,57}},
    {"Bulkheads",               {6500, 9000,350,900}},
    {"Belly Plates 3",          {7000, 8000,32,57}},
    {"Belly Plates 4",          {8000.0, 9800.0,45.0,90.0, 9801.0, 8000.0 - 1, 65.0, 70.0}},
    {"Collimator 5",            {7554, 7772, 57.01, 300 }},
    {"Collimator 6A",           {9550.0,9720.0,52,80.0}},
    {"Clamp 6A",                {9600.0,9750.0,100,455.0}},
    {"Lintel",                  {9600.49,9748.49, 475.0, 950.0}},
    {"Clamps 6B",               {10800.0,11180.0,161.0,650}},
    {"Collimator 6B",           {10925.0,11180.0,79.0,160.0}},
    {"Collar 1",                {11600.0,12000.00,540,760}},
    {"Vaccum Enclosure DS End", {12000, 12180, 700,1300}},
    {"Drift Pipe",              {13600, 19010, 1240,1300}},
    {"Bellows 4 Region",        {12200, 13600, 600,1300}},
    {"Vaccum Window",           {18400, 19010, 490, 1150}},
    {"Air Box 1",               {19011.0,20500.0,800.0,1009.0}},
    {"Air Box 2",               {19011.0,22500.0,640.0,800.0}},
    {"Bellows 5",               {19011.0,19299.0,400.0,640.0}},
    {"Det. Beam Pipe",          {19300.0,25000.0,500.0,640.0}},
    {"Collar 2",                {19011.0,19320.0,1009.0,1040.0}},
    {"Quartz",                  {20500,22500,800,1250}}
};

void init_histmap(const Boxes& boxes, Hist2Ds& hists){
    int i = 0;
    for(auto box : boxes){
        auto name = box.first; auto xy = box.second;
        auto get_th2d = [&xy, &i](std::string name){ return new TH2D(Form("vzvr%03d",i++),(name+ " ; vz[mm]; vr[mm]").c_str(),100,xy[0], xy[1], 100,xy[2], xy[3]); };
        auto get_xy  =  [&xy, &i](std::string name){ return new TH2D(Form("yx%03d",i++),(name+ " ; vx[mm]; vy[mm]").c_str(),100,-xy[3], xy[3],100,-xy[3],xy[3]); };
        //auto get_exy  =  [&xy, &i](std::string name){ return new TH2D(Form("yx%03d",i++),(name+ " ; x[mm]; y[mm]").c_str(),100,-xy[3], xy[3],100,-xy[3],xy[3]); };
        hists[name + " epm"] = get_th2d(name + " $e^{\\pm}$");
        hists[name + " epmxy"] = get_xy(name + " $e^{\\pm}$xy");
        //hists[name + " epmxye"] = get_xy(name + " $e^{\\pm}$ E");
        hists[name + " photon"] = get_th2d(name + " $\\gamma$");
        hists[name + " photonxy"] = get_xy(name + " $\\gamma$");
        //hists[name + " photonxye"] = get_xy(name + " $\\gamma$ E");
    }
}

bool in_qtiles(float vz, float vr){
    return utl::is_inside(vz,vr,{20500,22500,800,1250});
}

void count_box(RemollHit hit, const Boxes& boxes, Hist2Ds& hists){
    float vr = std::hypot(hit.vx, hit.vy); float vz = hit.vz;
    for(auto box : boxes){
        if(utl::is_inside(vz,vr,box.second) and !(box.first == "All Tight" and in_qtiles(vz,vr))) {
            std::string name;
            if(utl::cut::ring5_epm_E1(hit) and hit.pz > 0){
                hists[box.first + " epm"]->Fill(vz,vr);
                hists[box.first + " epmxy"]->Fill(hit.vx, hit.vy);
                //hists[box.first + " epmxye"]->Fill(hit.vx, hit.vy,hit.k);
            }
            else if (utl::cut::ring5_photon_E1(hit) and hit.pz > 0){
                hists[box.first + " photon"]->Fill(vz,vr);
                hists[box.first + " photonxy"]->Fill(hit.vx, hit.vy);
                //hists[box.first + " photonxye"]->Fill(hit.vx, hit.vy, hit.k);
            }
        }
    }
}

void save_hists(Hist2Ds& hists, const std::string& id, const std::string &opfile,float total=1e9){
    int i = 0;
    std::cout<<"Name, Count, z-pos, Moller Fraction"<<std::endl;
    for(auto hist: hists) {
        int count = hist.second->Integral();
        std::string name = hist.first;
        std::cout<<hist.first<<", "<<hist.second->GetXaxis()->GetXmin()<<", "<<count<<", "<<utl::moller_fraction(count,total)*100<<" "<<std::endl;
        utl::save(hist.second,Form("vz-vr-for-%03d",++i),id+"/plot",opfile);
    }
}

void count_boxx(hit_list hl, const Boxes& boxes, Hist2Ds& hists){
    for(auto hit: hl) count_box(hit, boxes, hists);
}

void fill_hist(hit_list hl, TH1D* histe, TH1D* histp, TH2D* exc){
    for(auto hit: hl){
        float vr = std::hypot(hit.vx, hit.vy); float vz = hit.vz;
        if(utl::cut::ring5_epm_E1(hit) and hit.pz > 0 and !in_qtiles(vz,vr)) histe->Fill(vz);
        if(utl::cut::ring5_photon_E1(hit) and hit.pz > 0 and !in_qtiles(vz,vr)) histp->Fill(vz);
        bool inside = false;
        for(auto box : components){
            if(utl::is_inside(vz,vr,box.second) and box.first != "All Tight" and box.first != "All") {
                inside = true;
                break;
            }
        }
        //if(!inside and utl::cut::ring5_epm_E1(hit) and hit.pz > 0) exc->Fill(vz,vr);
        if(utl::cut::ring5_epm_E1(hit) and hit.pz > 0) exc->Fill(vz,vr);
    }
}

void make_all_plots(const std::string& filename, const std::string& id, const std::string& opfile,float total=1e9){
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame d("T",filename);
    Hist2Ds histmap;

    auto the = new TH1D("vzhiste","All background $e^{\\pm}$; vz[mm]; count",200,0, 25000);
    auto thp = new TH1D("vzhistp","All background $\\gamma$; vz[mm]; count",200,0, 25000);
    auto exc = new TH2D("vzvrpl","All background; vz[mm]; vr[mm]", 200,0,25000.0,200, 0.0,1450.0);
    init_histmap(components, histmap);

    d.Foreach([&](hit_list hl)->void {
        fill_hist(hl, the,thp, exc);
        count_boxx(hl,components,histmap);
    },{"hit"});

    save_hists(histmap,id,opfile,total);
    utl::save(the,"vz-for-epm",id+"/plot1d",opfile);
    utl::save(thp,"vz-for-photon",id+"/plot1d",opfile);
    utl::save(exc,"exclusion-hotspots",id+"/plot1d",opfile);
}

void all_edges_background(std::string filename=example_file, float total=1e9, std::string opfile=opfile_ex, std::string id=opid){
    std::cout<<"# Reading :"<<filename<<std::endl;
    std::cout<<"# Writing :"<<opfile<<std::endl;
    std::cout<<"# ID is   :"<<id<<std::endl;
    std::cout<<"# Total is:"<<total<<std::endl;
    make_all_plots(filename, id, opfile,total);
    std::cout<<"# Written :"<<opfile<<std::endl;
}
