#include "utils.hh"

bool hit911(RemollHit hit)
{
    return  hit.det == 911;// || hit.det == 28;
}

/* Reads set of files whose path is provided as a line in the file `filelist` extracts
 * the hit branch from the file and writs the output to `ofilename` for which the hit
 * event passes the cut `cut_func` where the sinature of cut func is `bool cut_func(RemollHit)`
 * */
void merge_trees(std::string filelist,std::string ofilename)
{
    double total_hits = 0;
    hit_list newhit;
    std::function<bool(RemollHit)> cut = identity<RemollHit>;


    std::ifstream infile(filelist);
    std::string line;
    while (std::getline(infile, line))
    {
        std::cout<<"Reading file: "<<line<<std::endl;
        RemollData rd = RemollData(line,"T");
        std::vector<RemollHit> passed_hit = rd.get_values("hit",cut);
        size_t got_hits = passed_hit.size();
        std::cout<<got_hits<<" events passed the cut"<<std::endl;
        for(auto cur_hit : passed_hit)
            newhit.push_back(cur_hit);
        total_hits += got_hits;
    }


    TFile ofile = TFile(ofilename.c_str(),"RECREATE");
    TTree otree  = TTree("T", "skim tree");
    otree.Branch("hit", &newhit);
    ofile.cd();
    otree.Fill();
    otree.Write();
    ofile.Close();
    std::cout<<"Finished writing "<<total_hits<<" to "<<ofilename<<std::endl;
}



