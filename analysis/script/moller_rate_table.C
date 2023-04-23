#include <string>
#include <vector>

#include "RemollTree.hpp"
#include "misc_util.hh"
#include "localconfig.hh"

static const std::string opfile = "asset/files/sixab-all-info.root";

bool kind_cut(RemollHit hit, std::string kind){
    if(kind == "photon") return utl::cut::photon(hit);
    else return utl::cut::epm(hit);
}


void plot_hist(RemollTree& RT,std::string id, int ring = 0,std::string kind="epm"){

    auto cut = [ring,kind](RemollHit hit) { return utl::cut::md_ring(hit,ring) and kind_cut(hit,kind) and utl::cut::E1(hit);};
    auto vzhist = hst::vz(RT,cut,{-10000,30000},100,Form("Ring %d all %s; vz[mm]; Count",ring,kind.c_str())); vzhist->Draw();

    utl::save(vzhist,"ring-"+std::to_string(ring)+"-vz-"+kind,id+"/plot",opfile);
    delete vzhist;
}

void print_table(RemollTree& RT, std::string id, std::vector<double>& flpartition, std::string kind) {
    std::vector<int> rings={0,1,2,3,4,5,6};
    std::vector<hit_cut> ringcuts{};
    for(int ring : rings)
        ringcuts.push_back( [ring,kind](RemollHit hit)->bool { return utl::cut::md_ring(hit,ring) and utl::cut::E1(hit) and kind_cut(hit,kind); });
    //
    auto count_rings = msc::get_counts(RT,ringcuts,flpartition);
    utl::save(count_rings,"all-ring-counts-"+kind,id+"/data",opfile); //treename, data, branch,
    //
    std::vector<std::string> titles; for(int ring: rings) titles.push_back("Ring "+std::to_string(ring));
    msc::print_counts(count_rings,titles,1e8,flpartition);
}


void moller_rate_table(std::string filename="",int ir4=25,int ir6a=55,int ir6b=80){

    std::string id = std::string(Form("4IR%d6A%d6B%d",ir4,ir6a,ir6b));
    if(filename == "") filename = cfg::sixab::filename(ir4,ir6a,ir6b);
    printf("The filename is %s \n", filename.c_str());
    auto T = utl::get_tree(filename);
    RemollTree RT(T,{"hit"});

    std::vector<double> flpartition{-1e5,-3700,2000,8500,14500,22200, 35000,1e6};

    for(std::string kind : {"photon", "epm"}){
        for(int ring : {0, 5})
            plot_hist(RT,id,ring,kind);

        for(auto v: flpartition) utl::add_vline(v);
        print_table(RT,id,flpartition,kind);
    }

}

/* Params

std::string filename = ""; int ir4 = 40, ir6a =55, ir6b = 80;
std::string kind = "epm"; int ring = 5;

*/

