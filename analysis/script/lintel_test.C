#include "utils.hh"

template <typename T,int det,int pid> bool det_pid(T obj) {
    return obj.det == det && (obj.pid == pid || obj.pid == -pid);
}

void fill_hist(TH2D* hist2d, TTree* T,std::function<bool(RemollHit)> cut,bool rateweight=false) {
    hit_list  *hits=0; double rate;
    std::cout<<"Filling hist"<<std::endl;
    T->SetBranchAddress("hit",&hits);
    T->SetBranchAddress("rate",&rate);
    int totentry = T->GetEntries();
    for(int entry = 0; entry <= totentry; ++entry) {
        T->GetEntry(entry);
        //for(int i = 0; i < hits->size(); ++i)
        for(auto hit: *hits) {
            if(!rateweight) rate = 1;
            if( cut(hit) ) hist2d->Fill(hit.x,hit.y,rate);
        }
    }
}


TChain* make_chain(std::string filelist){
    std::ifstream flist(filelist);
    std::string filename;
    TChain* TC = new TChain("T");
    while(std::getline(flist,filename)){
        std::cout<<"Adding "<<filename<<std::endl;
        TC->Add(filename.c_str());
    }
    return TC;
}


void lintel_test(std::string filelist) {
    TCanvas* c1 = new TCanvas("c1","c1");
    TChain* lintels = make_chain(filelist);
    TH2D* xyhist = new TH2D("Lintel","Lintel exit",120,-850,-150,120,0,120);
    fill_hist(xyhist,lintels,det_pid<RemollHit,67,11>,false);
    xyhist->Draw("colz");
    xyhist->GetXaxis()->SetTitle("x [mm]");
    xyhist->GetYaxis()->SetTitle("y [mm]");
    c1->SaveAs("asset/image/lintel/Target.pdf");
}


int main(){

    std::string basedir = "/home/pranphy/simana/sim/output/no-lintel-20230206-155101/primary/";
    std::string filelist = basedir+"filelist.txt";
    lintel_test(filelist);
    return 0;

}
