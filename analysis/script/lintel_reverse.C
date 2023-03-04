#include <string>
#include "TCanvas.h"
#include "TChain.h"
#include "TH2D.h"

#include "utils.hh"

void get_lintel_coordinates(std::vector<double>& x, std::vector<double>& y, char face = 'f'){
    if (face == 'f'){
        x =  {-500.85,-806.85,-806.85,-484.85,-500.85}; //front face x coordinates
        y = {140,100,0,0,140};                         // front face y coordinates
    } else {
        x = {-505.85,-806.85,-806.85,-489.85,-505.85}; // back face x coordinates
        y = {140,100,0,0,140};                         // back face y coordinates
    }
}


bool lintel_pos_cut(RemollHit hit){
    return hit.x > -805 && hit.x < -150 &&  0.0 <= hit.y  && hit.y <= 150;
}

/* reverse_lookup: Given a list of hits, select those hits with `hit.det == from`,
   for those events find the trackid and look up event in the list of hits again
   to find if that track also has a hit at `hit.det==to`. If so, return the list
   of such hits at `to`.
   Now accepts `ring` parameter which applies the radial cut for various numbered
   rings. The rings radii are defined for MOLLER detector, but  could be thought
   as any regular radial cut. HA HA
*/
hit_list reverse_lookup(hit_list hits,int from, int to,int ring=0) {
    std::vector<int> trids;
    std::vector<RemollHit>  rev_hits(0);
    for(auto hit: hits){
        if(hit.det == from &&  utl::md_ring_cut(hit,ring) ){
            trids.push_back(hit.trid);
        }
    }
    for(auto hit: hits) {
        if(hit.det == to && hit.e > 1.0 ) {
            auto where = std::find(trids.begin(),trids.end(),hit.trid);
            if(where != trids.end()) rev_hits.push_back(hit);
        }
    }
    return rev_hits;
}

void add_lintel_outline(TCanvas* canvas,char face = 'f'){ // The lintel coordinates are for the front face of lintel from hybridDaugher_unified.gdml
    std::vector<double> x, y;
    get_lintel_coordinates(x,y,face);
    TPolyLine* poly = new TPolyLine(x.size(),x.data(),y.data());
    poly->SetLineWidth(3); poly->SetLineColor(kRed); poly->Draw();
    canvas->Update();
}


std::pair<double,double> get_xy(RemollHit hit) {
    return std::pair<double,double>{hit.x,hit.y};
}

void update_histogram(TH2D* hist,double xmin,double xmax,double ymin,double ymax,double vmin,double vmax){
    hist->SetStats(kFALSE);
    //hist->SetAxisRange(vmin, vmax, "Z");
    //hist->SetAxisRange(xmin, xmax, "X");
    //hist->SetAxisRange(ymin, ymax, "Y");
    hist->GetXaxis()->SetTitle("x [mm]");
    hist->GetYaxis()->SetTitle("y [mm]");
    hist->Draw("colz");
}

double fill_hist2d(TH2D* hist2d, pair_func get_xy, TTree* T, std::function<hit_list(hit_list)> lookup, std::function<bool(RemollHit)> cut, bool rateweight=false) {
    hit_list  *hits=0; double rate;
    T->SetBranchAddress("hit",&hits);
    T->SetBranchAddress("rate",&rate);
    int totentry = T->GetEntries();
    double calc_rate = 0;
    for(int entry = 0; entry <= totentry; ++entry) {
        T->GetEntry(entry);
        //if(rate > 1e10) continue; // seems to me that 1e11 is the bogus rate value.
        hit_list rev_hits = lookup(*hits);
        if(!rateweight) rate = 1;
        bool flag = false;
        for(RemollHit hit: rev_hits ) {
            auto xyp = get_xy(hit);
            double x = xyp.first; double y = xyp.second;
            if( cut(hit) ) {
                flag = true;
                calc_rate += rate;
                hist2d->Fill(x,y,rate);
            }
        }
        //if(flag) calc_rate += rate;
    }
    return calc_rate;
}

void lintel_reverse(std::string filelist,int detid,int ring = 0, std::string weight="yes",  std::string strategy="normal", std::string id="default") {
    TChain* lintels = utl::make_chain(filelist);
    int nof = lintels->GetListOfFiles()->GetSize();
    TCanvas* c1 = new TCanvas("c1","c1");
    c1->SetFillColor(TColor::GetColor(190,190,190));
    std::string title = detid == 65 ? "Lintel Entry" : "Lintel Exit";
    if(strategy == "reverse") {title += ring == 5 ? " -> Ring 5 at MD " : " -> All Rings at MD";}
    std::cout<<"title :"<<title<<std::endl;
    TH2D* xyhist = new TH2D("Lintel",title.c_str(),120,-850,-150,120,0,150);
    //TH2D* xyhist = new TH2D("Lintel",title.c_str(),810,-850,850,810,-850,850);
    auto electron_at_lintel = [&](RemollHit h)->bool{return utl::det_pid_cut(h,detid,PID::ELECTRON);};// && lintel_pos_cut(h) ;};
    typedef std::function<hit_list(hit_list)> lookup_func; // hit_list is a typedef for std::vector<RemollHit>
    lookup_func lintel_to_md = [&](hit_list hl)->hit_list{return reverse_lookup(hl,DETID::MD,detid,ring);};
    lookup_func all_events = [&](hit_list hl)->hit_list{return hl;};
    lookup_func lookup =  strategy == "normal" ? all_events : lintel_to_md;
    double integral = fill_hist2d(xyhist,get_xy,lintels,lookup,electron_at_lintel,weight=="yes");
    double all_lintel_rate = (integral/nof)/1e9; // 14 because this is only half of lintel and there are 7 lintels with make 14 halves
    std::cout<<"Rate :: "<<all_lintel_rate<<std::endl;
    update_histogram(xyhist,-850,-150,0,150,0,1e7);
    utl::show_text(Form("Rate = %2.3f GHz ",all_lintel_rate),0.8,0.825,0.03);
    add_lintel_outline(c1,detid == 65 ? 'f' : 'b'); // front face for 65 and back fece for 67, these are detids for them
    std::string opfilename = "asset/image/lintel/plot-DC-RATE-vmin-vmax-"+id+"-"+std::to_string(detid)+"-"+strategy+"-ring-"+std::to_string(ring)+".pdf";
    //std::string opfilename = "asset/image/lintel/geometry/one-slice-full-plot-without-lintel.pdf";
    c1->SaveAs(opfilename.c_str());
}


int main(){
    /*
    mystyle()
    TCanvas* c1 = new TCanvas("c1","c1",800,600);
    std::string ofilename="oh-hello.txt";
    std::string basedir = "/home/pranphy/simana/sim/output/yes-lintel-20230206-155101/primary/";
    std::string filelist = basedir+"filelist.txt";
    int detid = 67;
    std::string strategy = "normal";
    std::string weight = "yes";
    std::string id = "repl-bata";
    int ring = 5
    lintel_reverse(filelist,detid,ring,weight,strategy,id);
    add_lintel_outline(c1);
    c1->SaveAs("/tmp/lintel/lintel-test.tex")
    */
    return 0;
}
