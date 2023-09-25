#include <string>
#include <vector>

#include "ROOT/RDataFrame.hxx"
#include "utils.hh"
#include "misc_ntq.hh"

static const std::string opfile = "asset/files/root/sixab-all-info-01_del.root";

bool kind_cut(RemollHit hit, std::string kind){
    if(kind == "photon") return utl::cut::photon(hit);
    else return utl::cut::epm(hit);
}

void update_counts(hit_list& hl, std::vector<std::vector<hit_cut>>& cuts,std::vector<std::vector<int>>& counts){
    std::vector< std::vector<int> > count_cuts;
    for(auto& hit: hl){
        for(size_t i=0; i < cuts.size(); ++i){ // partitions
            auto ringcuts = cuts[i];
            for(size_t j = 0; j < ringcuts.size(); ++j){ // rings
                auto cut = ringcuts[j];
                if( ringcuts[j](hit) ) { ++counts[i][j]; }
            }
        }
    }
}


void print_table(ROOT::RDataFrame rd, std::string id, std::vector<double>& flpartition, std::string kind) {
    std::vector<int> rings={0,1,2,3,4,5,6};
    std::vector<std::vector<hit_cut>> cutcuts{};
    std::vector<std::vector<int>> vzring_counts{};
    for(size_t i = 1; i < flpartition.size(); ++i){
        double minvz = flpartition[i-1], maxvz = flpartition[i];
        std::vector<hit_cut> ringcuts{};
        std::vector<int> counts{};
        for(int ring : rings){
            ringcuts.push_back( [ring,kind,minvz,maxvz](RemollHit hit)->bool {
                    return utl::cut::md_ring(hit,ring) and utl::cut::E1(hit) and kind_cut(hit,kind) and hit.pz > 0
                    and hit.vz > minvz and hit.vz <= maxvz; 
            });
            counts.push_back(0);
        }
        cutcuts.push_back(ringcuts);
        vzring_counts.push_back(counts);
    }

    rd.Foreach([&](hit_list hl){
        update_counts(hl,cutcuts, vzring_counts);
    },{"hit"});

    utl::save(vzring_counts,"all-ring-counts-"+kind,id+"/data",opfile); //treename, data, branch,

    std::vector<std::string> titles; for(int ring: rings) titles.push_back("Ring "+std::to_string(ring));
    msc::print_counts(vzring_counts, titles,1e8,flpartition);
}


void moller_rate_table(std::string filename="",std::string id=""){
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame d("T",filename);
    if(id == "") id = "test-babies";

    printf("The filename is %s \n", filename.c_str());

    std::vector<double> flpartition{-1e5,-3700,2000,8500,14500,18932,19290,22200, 35000,1e6};

    for(std::string kind : {"photon", "epm"}){
        for(auto v: flpartition) utl::add_vline(v);
        print_table(d,id,flpartition,kind);
    }

}
