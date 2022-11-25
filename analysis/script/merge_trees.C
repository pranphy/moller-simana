#include "utils.hh"

bool cut_func(RemollHit hit)
{
    return  hit.det == 911;
}

double merge_trees(std::string filelist,std::string ofilename)
{
    double total_hits = 0;
    hit_list* newhit = 0;

    TFile* ofile = new TFile(ofilename.c_str(),"RECREATE");

    TTree* otree  = new TTree("T", "skim tree");
    otree->Branch("hit", &newhit);

    std::ifstream infile(filelist);
    std::string line;
    while (std::getline(infile, line))
    {
        std::cout<<"Reading file: "<<line<<std::endl;
        RemollData rd = RemollData(line,"T");
        double got_hits = rd.push_hit_to(&newhit,cut_func);
        total_hits += got_hits;
    }


    ofile->cd();
    otree->Fill();
    otree->Write();
    otree->Print();
    ofile->Close();
    return total_hits;
}


