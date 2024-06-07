// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=cpp
// author : Prakash [प्रकाश]
// date   : 2023-06-24

#include <string>

#include "RemollTree.hpp"


std::string get_skim(std::string id){
    id = "col2-cfg06-sec01/" + id;
    return "sim/output/beam-col2-cfg06/secondary/"+id+"/skim/merged/events-at-least-1-hit-md.root";
}

int cct = 0;

typedef std::vector<float> Box;

bool from_vzvr(RemollHit hit, Box box){
    float vr = utl::hypot(hit.vx,hit.vy); // from signifies vz vr
    return utl::is_inside((float)hit.vz,vr,box);
}

std::pair<int,int> loop_count(TTree* T, hit_cut cut){
    RemollTree RT(T,{"hit"});
    int count1 = 0, count2  = 0;
    auto fill_count = [&](RemollHit hit){ if(cut(hit)) { if(hit.trid==1) ++count1; else ++count2;} };
    loop_tree(RT,fill_count);
    auto cn = new TCanvas("cn","");
    auto h2 = new TH2D("vzvr","VzVr at Col2 ",100, 19100,19300,100,1008,1024);
    T->Draw("utl::hypot(hit.vx,hit.vy):hit.vz>>vzvr","hit.det == 28 && abs(hit.pid) == 11 && hit.e > 1 && hit.r > 930 && hit.r < 1070 && hit.trid != 1","colz");
    cn->SaveAs(Form("more-vzvr-%d.pdf",++cct));
    return {count1,count2};
}

void change_col2_angle(){

    std::vector<std::string> angid={
        "c2-ang-3.40", "c2-ang-3.44", "c2-ang-3.50", "c2-ang-4.00",
         "c2-ang-4.10","c2-ang-4.20","c2-ang-4.30","c2-ang-4.40",
        "c2-ang-4.50", "c2-ang-5.00",
    };

    std::map<std::string,TTree*> ir_trees;
    for(auto id: angid) ir_trees.insert({id,utl::get_tree(get_skim(id))});

    float skim_count = 703, secondary = 1e6, primary=100e6;

    auto mf= [&](float count){ return utl::moller_fraction(count,secondary,skim_count,primary)*100; };

    Box boxc2 = {19100,19300, 1010, 1024};

    std::vector<hit_cut> cuts={ utl::cut::ring5_epm_E1, utl::cut::ring5_photon_E1 };
    for(auto basecut: cuts){
        printf(" cut \n");
        for(auto box : {boxc2}){
            utl::println(box);
            printf(" trid  != 1           trid == 1       total        id  \n");
            for(std::string id : angid){
                auto cut = [&](RemollHit hit){ return basecut(hit) && from_vzvr(hit,box); };
                auto counts = loop_count(ir_trees[id],cut);
                float mf1 = mf(counts.first), mf2 = mf(counts.second), mft = mf(counts.first+counts.second);
                //printf(" tird1 =  %5d (%5.3f%%); trid != 1 = %5d (%5.3f%%); total = %5d (%5.3f%%) :%-20s \n",counts.first,mf1,counts.second,mf2,counts.first+counts.second,mft,id.c_str());
                printf("%5d (%5.3f%%);  %5d (%5.3f%%); %5d (%5.3f%%) :%-20s \n",counts.second,mf2,counts.first,mf1,counts.first+counts.second,mft,id.c_str());
            }
        }
    }


}

