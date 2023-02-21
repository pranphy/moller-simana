#include "utils.hh"

TCanvas* c1 = new TCanvas("c1","c1");

std::vector<std::string> cuts = {
    "hit.det == 28 && hit.pid == 11 && hit.r > 930 && hit.r < 1070 ",
    "hit.det == 28 && hit.pid == 11 && hit.r > 930 && hit.r < 1070 && hit.e > 1",
    "hit.det == 28 && hit.pid == 11 && hit.r > 930 && hit.r < 1070 && hit.e > 1 && (hit.trid == 1 || hit.trid ==2 )"
};

void get_rate_from_histogram(TTree* files,std::string cut,int nof){
    c1->Clear();
    files->Draw("rate>>htmp",cut.c_str());
    TH1D* htmp = (TH1D*) c1->GetPrimitive("htmp");
    double mean = htmp->GetMean();
    double entries = htmp->GetEntries();
    double hrate = (mean*entries/nof)/1e9; //GHz
    //printf("The rate my friend is: %.3fGHz, and not blowing in the wind",hrate)
    printf("%.3fGHz :: %s \n",hrate,cut.c_str());
}

void calc_rate_from_sim(std::string filelist){
    TChain* files = utl::make_chain(filelist);
    int nof = files->GetListOfFiles()->GetSize();
    for (auto cut: cuts){
        get_rate_from_histogram(files,cut,nof);
    }
}

std::vector<std::string> simulations = {
    "/w/halla-scshelf2102/moller12gev/zdemirog/moller/config36_wNewMagneticFiles/remoll/analysis/Moller.txt",
    "/w/halla-scshelf2102/moller12gev/pgautam/pf/simana/sim/output/no-lintel-20230206-155101/primary/filelist.txt"
};

void do_all(){
    for(auto sim: simulations){
        printf("\n %s \n",sim.c_str());
        calc_rate_from_sim(sim);
    }
}



void rate_with_hist(){
    do_all();
}
