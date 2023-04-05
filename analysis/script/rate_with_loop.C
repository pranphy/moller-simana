#include "utils.hh" // Some utilities stuffs
#include "RemollTree.hpp" // Easy looping Remoll tree and chain

typedef std::function<bool(RemollHit)> cut_func;

bool ring_five(RemollHit hit){ return hit.r > 930 && hit.r < 1070;}
bool electron_at_md(RemollHit hit){ return (hit.pid == PID::ELECTRON || hit.pid == PID::POSITRON) && hit.det == DETID::MD;} // PID::ELECTRON = 11 and DETID::MD =  28
bool min_energy(RemollHit hit){ return hit.e > 1; } // 1 MeV
bool primary_tracks(RemollHit hit){ return hit.trid == 1 || hit.trid == 2; }

std::vector<cut_func> cuts = {
    [](RemollHit hit)->bool { return ring_five(hit) && electron_at_md(hit); },
    [](RemollHit hit)->bool { return ring_five(hit) && electron_at_md(hit) && min_energy(hit); },
    [](RemollHit hit)->bool { return ring_five(hit) && electron_at_md(hit) && min_energy(hit) && primary_tracks(hit); },
};

//void get_rate_from_histogram(TTree* files,std::string cut,int nof){
double get_rate_from_loop(TTree* files, cut_func cut,int nof,bool doublecount=false){
    double lrate = 0;
    RemollTree RT(files);
    for(RT.loop_init(); RT.next();){
        bool atleast_one_hit = false;
        for(auto hit: *RT.cur_hits){
            if(cut(hit)){
                atleast_one_hit = true;
                if (doublecount)  lrate += RT.cur_rate;
            }
        }
        if(!doublecount && atleast_one_hit) lrate += RT.cur_rate;
    }
    lrate = (lrate/nof);
    return lrate;
}

void calc_rate_from_sim(std::string filelist,bool doublecount=false){
    TChain* files = utl::make_chain(filelist);
    int nof = files->GetListOfFiles()->GetSize();
    for (auto cut: cuts){
        double lrate = get_rate_from_loop(T,cut,nof,true);
        printf("%.3fGHz\n",lrate/1e9);
    }
}

std::vector<std::string> simulations = {
    "/w/halla-scshelf2102/moller12gev/zdemirog/moller/config36_wNewMagneticFiles/remoll/analysis/Moller.txt",
    "/w/halla-scshelf2102/moller12gev/pgautam/pf/simana/sim/output/no-lintel-20230206-155101/primary/filelist.txt"
    //"/mnt/stg/data/remoll/sim/output/no-lintel-20230206-155101/primary/filelist.txt",
    //"/mnt/stg/data/remoll/sim/output/yes-lintel-20230206-155101/primary/filelist.txt"
};

void do_all(bool doublecount){
    for(auto sim: simulations){
        std::cout<<"Filelist ::"<<sim<<std::endl;
        calc_rate_from_sim(sim,doublecount);
    }
}


void rate_with_loop(std::string filelist="", std::string method="single"){
    if(filelist == "") do_all(method=="double"); // if no files supplied do all in simulations vector
    else calc_rate_from_sim(filelist,method=="double");
}

