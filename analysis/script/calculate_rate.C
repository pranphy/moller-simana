#include "utils.hh" // Some utilities stuffs
#include "ROOT/RDataFrame.hxx"

std::string  default_filelist{"/lustre19/expphy/volatile/halla/moller12gev/pgautam/sim_data/beam-atmosphere-cfg04/primary/filelist.txt"};

double get_rate(ROOT::RDataFrame& df, hit_cut cut,int nof){
    double lrate = 0;
    df.Foreach([&lrate,&cut](hit_list hits, double rate)->void{
        for(auto& hit: hits) if(cut(hit)) lrate += rate;
    },{"hit","rate"});
    lrate = (lrate/nof);
    return lrate;
}


void calculate_rate(std::string filelist=""){
    if(filelist == "") filelist = default_filelist;


    auto files = utl::readlines(filelist);
    int nof = files.size();

    /**
      cut is a function that takes a hit and decides whether to count it or not.

    */
    auto cut = utl::cut::ring5_epm;

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("T",files);


    double lrate = get_rate(df,cut,nof);
    printf("%.3fGHz\n",lrate/1e9);
}

