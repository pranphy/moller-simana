#include "RemollTree.hpp"

bool test_cur(RemollHitPartRate ){
    return true;
}

template<typename T> void print_pids(std::vector<T> objs){
    std::for_each(objs.begin(),objs.end(),[&](T a){std::cout<<a.pid<<",";});
}

bool lintel_pos_cut(RemollHit hit){
    return (hit.x > -510 && hit.x < -480 && hit.y > 0 && hit.y < 150 && hit.det == 65);
}

void fill_hist(RemollTree& RT,TH1D* hist1d) {
    hit_list cur_hits = *RT.cur_hits;
    for(auto hit: cur_hits){
        if(hit.pid == 11) {
            double ang = acos(hit.pz/sqrt(hit.px*hit.px+hit.py*hit.py+hit.pz*hit.pz));
            std::cout<<hit.x<<","<<hit.y<<","<<hit.z<<","<<ang<<std::endl;
            hist1d->Fill(ang);
        }
    }
}

void angular_distribution_at_lintel(std::string filelist) {
    TCanvas* c1 = new TCanvas("c1","c1");

    std::cout<<"I am here "<<std::endl;
    TChain* lintels =  utl::make_chain(filelist);
    RemollTree RT(lintels);
    //std::string title = R"($$cos^{-1}\left(\frac{p_z}{\sqrt{p_x^2+p_y^2+p_z^2}}\right)$$)";
    std::string title = "Angular distribution at Lintel Plane";
    TH1D* AngHist = new TH1D("Lintel Plane",title.c_str(),320,0.0,0.10);
    RT.loop_init();
    std::cout<<"Chain got"<<RT.totentries<<" entries "<<std::endl;
    int cnt = 0;

    while(RT.next()){
        hit_list cur_hits = *RT.cur_hits;
        part_list cur_parts = *RT.cur_parts;
        fill_hist(RT,AngHist);
    }
    AngHist->SetStats(kFALSE);
    AngHist->Draw();

    AngHist->GetXaxis()->SetTitle("angle radian");
    AngHist->GetYaxis()->SetTitle("PDF (arb. unit.)");
    c1->SaveAs("asset/image/lintel/real-angle-all-plane.png");
}

int main(){
    std::string basedir = "/home/pranphy/simana/sim/output/no-lintel-20230206-155101/primary/";
    std::string filelist = basedir+"filelist.txt";
    angular_distribution_at_lintel(filelist);
    return 0;
}

