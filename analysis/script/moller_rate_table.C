#include <string>
#include <vector>

#include "RemollTree.hpp"
#include "misc_util.hh"

void moller_rate_table(std::string filename){

    //std::string filename="sim/output/beam-NOQTZ-6AIR606BIR90IA2.62e-2-202304141213/primary/skim/merged/events-at-least-hitting-noepm.root";
    auto T = utl::get_tree(filename);
    RemollTree RT(T,{"hit"});

    std::vector<double> flpartition{-1e5,-3700,8500,14500,22200, 35000,1e6};
    auto vzhist = hst::vz(RT,utl::cut::ring5_epm_E1,{-10000,30000},100,"Ring 5 all epm > 1MeV; vz[mm]; Count"); vzhist->Draw();
    for(auto v: flpartition) utl::add_vline(v);

    std::vector<hit_cut> ringcuts; std::vector<int> rings{0,3,4,5};
    for(int ring : rings){
        ringcuts.push_back([ring](RemollHit hit)->bool {
                return utl::cut::md_ring(hit,ring) and utl::cut::E1(hit) and utl::cut::epm(hit); 
        });
    }
    auto count_rings = msc::get_counts(RT,ringcuts,flpartition);
    std::vector<std::string> titles; for(int ring: rings) titles.push_back(std::string(Form("Ring %d",ring)));
    msc::print_counts(count_rings,titles,1e8,flpartition);

}

