#include <string>
#include <vector>

#include "ROOT/RDataFrame.hxx"
#include "utils.hh"
#include "misc_ntq.hh"

static const std::string opfile = "asset/files/root/sixab-all-info-02_tt.root";
typedef std::vector<std::vector<hit_cut>> cut_table;
typedef std::vector<std::vector<int>> hit_table;

bool kind_cut(RemollHit hit, std::string kind){
    if(kind == "photon") return utl::cut::photon(hit);
    else return utl::cut::epm(hit);
}

void fill_hist(hit_list& hl, TH1D* epmhist, TH1D* photonhist){
    for(auto& hit: hl){
        if( utl::cut::ring5_epm_E1(hit) ) epmhist->Fill(hit.vz);
        if( utl::cut::ring5_photon_E1(hit) ) photonhist->Fill(hit.vz);
    }
}

void update_counts(hit_list& hl, std::vector<cut_table>& cuts,std::vector<hit_table>& counts){
    std::vector< std::vector<int> > count_cuts;
    for(auto& hit: hl){
        for(size_t i=0; i < cuts.size(); ++i){ // partitions
            auto krcuts = cuts[i];
            for(size_t j = 0; j < krcuts.size(); ++j){ // rings
                auto ringcuts = krcuts[j];
                for(size_t k=0; k < ringcuts.size(); ++k){
                    if( ringcuts[k](hit) ) { ++counts[i][j][k]; }
                }
            }
        }
    }
}


void print_table(ROOT::RDataFrame rd, std::string id, std::vector<double>& flpartition) {
    std::vector<int> rings={0,1,2,3,4,5,6};
    std::vector<cut_table> cutcuts{}; std::vector<hit_table> vzring_counts{};
    auto vzepm = new TH1D("vzepm","Ring 5 all epm",100,-10000,30000);
    auto vzphoton = new TH1D("vzepm","Ring 5 all photon",100,-10000,30000);
    for(std::string kind : {"photon", "epm"}){
        hit_table kind_hits{}; cut_table kind_cuts{};
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
            kind_cuts.push_back(ringcuts);
            kind_hits.push_back(counts);
        }
        cutcuts.push_back(kind_cuts);
        vzring_counts.push_back(kind_hits);
    }


    rd.Foreach([&](hit_list hl){
        update_counts(hl,cutcuts, vzring_counts);
        fill_hist(hl,vzepm, vzphoton);
    },{"hit"});

    utl::save(vzring_counts[0],"all-ring-counts-photon",id+"/data",opfile);
    utl::save(vzring_counts[1],"all-ring-counts-epm",id+"/data",opfile);
    utl::save(vzepm,"ring-5-vz-epm",id+"/plot",opfile);
    utl::save(vzphoton,"ring-5-vz-photon",id+"/plot",opfile);

    std::vector<std::string> titles; for(int ring: rings) titles.push_back("Ring "+std::to_string(ring));
    msc::print_counts(vzring_counts[0], titles,1e8,flpartition);
    msc::print_counts(vzring_counts[1], titles,1e8,flpartition);
}

void moller_rate_table(std::string filename="",std::string id=""){
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame d("T",filename);
    if(id == "") id = "test-babies";

    printf("The filename is %s \n", filename.c_str());

    //std::vector<double> flpartition{-1e5,-3700,2000,8500,14500,18932,19290,22200, 35000,1e6};
    std::vector<double> flpartition{-1e5,-3700,2000,8500,14500,22200, 35000,1e6};

    print_table(d,id,flpartition);

}
