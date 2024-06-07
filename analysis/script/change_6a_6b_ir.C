#include <string>

#include "RemollTree.hpp"


std::string get_skim(std::string id){
    id = "angles-6as/" + id;
    //return "sim/output/real-assymetric-202303271207/secondary/"+id+"/skim/merged/events-at-least-1-hit-md.root";
    return "sim/output/beam-dmap-bp-4IR256AIR556BIR80-202305021158/secondary/"+id+"/skim/merged/events-at-least-1-hit-md.root";
}

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
    return {count1,count2};
}

void change_6a_6b_ir(){

    std::vector<std::string> angid={
        "sec-0.40-1.00-4IR256AIR55.316BIR80.00","sec-0.50-1.00-4IR256AIR55.316BIR80.00",
        "sec-0.60-1.00-4IR256AIR55.316BIR80.00","sec-0.80-1.00-4IR256AIR55.316BIR80.00",
        "sec-0.90-1.50-4IR256AIR55.316BIR80.00","sec-1.00-1.50-4IR256AIR55.316BIR80.00",
        "sec-1.50-1.50-4IR256AIR55.316BIR80.00","sec-2.18-1.50-4IR256AIR55.316BIR80.00"
    };
    //for(int ir6b : {90,95,100}) angid.push_back(std::string(Form("6AIR656BIR%dIA2.62e-2",ir6b)));
    //for(float angle : {0.4,0.5,0.6,0.8}) angid.push_back(std::string(Form("sec-%.2f-%.2f-4IR256AIR55.316BIR80.00",angle,1.0)));
    //for(float angle : {0.7,0.85,1.0}) angid.push_back(std::string(Form("sec-%.2f-%.2f-4IR256AIR55.316BIR80.00",0.6,angle)));

    std::map<std::string,TTree*> ir_trees;
    for(auto id: angid) ir_trees.insert({id,utl::get_tree(get_skim(id))});

    float skim_count = 27598, secondary = 2e6, primary=100e6;

    auto mf= [&](float count){ return utl::moller_fraction(count,secondary,skim_count,primary)*100; };

    Box box6a = {9552.0, 9720, 55, 60.0};
    Box box6b = {10900.0, 11110, 78.0, 95.0};
    Box boxboth = {9552.0,11110.0,55.0,95.0};
    //Box boxboth = {8500.0,14500.0,0.0,2505.0};

    std::vector<hit_cut> cuts={ utl::cut::ring5_epm_E1, utl::cut::ring5_photon_E1 };
    for(auto basecut: cuts){
        printf(" cut \n");
        for(auto box : {box6a, box6b, boxboth}){
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

/*
void other_stuffs(){

    auto T = ir_trees["sec-0.60-1.00-4IR256AIR55.316BIR80.00"];
    RemollTree RT(T,{"hit"});

    auto ntrid = [](RemollHit hit)->bool{return utl::cut::ring5_epm_E1(hit) && hit.trid != 1;};
    auto sixahist = hst::vzvr(RT,ntrid,box6b,100,"Sixa");
    sixahist->Draw();

}
*/


