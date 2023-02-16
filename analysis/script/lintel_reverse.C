#include <string>
#include "TCanvas.h"
#include "TChain.h"
#include "TH2D.h"

#include "utils.hh"


bool md_ring_cut(RemollHit hit, int ring = 0){
    int rmin = 0; int rmax = 0; int offset = 10; // Recheck offset
    if(ring == 0){ rmin = 650; rmax = 1160; } // ring = 0 means no ring cut
    else if(ring == 1) {rmin = 650; rmax = 680;} // Table from  DocDB 896-v1
    else if(ring == 2) {rmin = 680; rmax = 740; }
    else if(ring == 3) {rmin = 740; rmax = 800; }
    else if(ring == 4) {rmin = 800; rmax = 920; }
    else if(ring == 5) {rmin = 920; rmax = 1060; }
    else if(ring == 6) {rmin = 1060; rmax = 1160; }
    return (hit.r > rmin+offset) && (hit.r <= rmax+offset);
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
        if(hit.det == from &&  md_ring_cut(hit,ring) ){
            trids.push_back(hit.trid);
        }
    }
    for(auto hit: hits) {
        if(hit.det == to && hit.e > 1.0) {
            auto where = std::find(trids.begin(),trids.end(),hit.trid);
            if(where != trids.end()) rev_hits.push_back(hit);
        }
    }
    return rev_hits;
}

void add_lintel_outline(TCanvas* canvas,char face = 'f'){ // The lintel coordinates are for the front face of lintel from hybridDaugher_unified.gdml
    Double_t xf[] = {-500.85,-806.85,-806.85,-484.85,-500.85}; //front face x coordinates
    Double_t yf[] = {140,100,0,0,140};                         // front face y coordinates
    Double_t xb[] = {-505.85,-806.85,-806.85,-489.85,-505.85}; // back face x coordinates
    Double_t yb[] = {140,100,0,0,140};                         // back face y coordinates
    Double_t* x; Double_t* y;
    if (face == 'f') {x = xf; y = yf;}
    else { x = xb; y = yb;}
    TPolyLine* poly = new TPolyLine(5,x,y);
    poly->SetLineWidth(3); poly->SetLineColor(kRed); poly->Draw(); canvas->Update();
}


std::pair<double,double> get_xy(RemollHit hit) {
    return std::pair<double,double>{hit.x,hit.y};
}

void lintel_reverse(std::string filelist,int detid,int ring = 0, std::string weight="yes",  std::string strategy="normal", std::string id="default") {
    TChain* lintels = utl::make_chain(filelist);
    TCanvas* c1 = new TCanvas("c1","c1");
    std::string title = detid == 65 ? "Lintel Entry" : "Lintel Exit";
    if(strategy == "reverse") {title += ring == 5 ? " -> Ring 5 at MD " : " -> All Rings at MD";}
    std::cout<<"title :"<<title<<std::endl;
    TH2D* xyhist = new TH2D("Lintel",title.c_str(),120,-850,-150,120,0,150);
    auto lintel_exit_electron = [&](RemollHit h)->bool{return utl::det_pid_cut(h,detid,PID::ELECTRON);};
    typedef std::function<hit_list(hit_list)> lookup_func; // hit_list is a typedef for std::vector<RemollHit>
    lookup_func lintel_to_md = [&](hit_list hl)->hit_list{return reverse_lookup(hl,DETID::MD,detid,ring);};
    lookup_func all_events = [&](hit_list hl)->hit_list{return hl;};
    lookup_func lookup =  strategy == "normal" ? all_events : lintel_to_md;
    utl::fill_hist2d(xyhist,get_xy,lintels,lookup,lintel_exit_electron,weight=="yes");
    xyhist->SetStats(kFALSE);
    xyhist->Draw("colz");
    xyhist->GetXaxis()->SetTitle("x [mm]");
    xyhist->GetYaxis()->SetTitle("y [mm]");
    add_lintel_outline(c1,detid == 65 ? 'f' : 'b'); // front face for 65 and back face for 67, these are detids for them
    std::string opfilename = "asset/image/lintel/plot-"+id+"-"+std::to_string(detid)+"-"+strategy+"-ring-"+std::to_string(ring)+".pdf";
    c1->SaveAs(opfilename.c_str());
}


int main(){
    /*
    TCanvas* c1 = new TCanvas("c1","c1");
    std::string ofilename="oh-hello.txt";
    std::string basedir = "/home/pranphy/simana/sim/output/no-lintel-20230206-155101/primary/";
    std::string filelist = basedir+"filelist.txt";
    int detid = 65;
    std::string strategy = "reverse";
    std::string weight = "no";
    std::string id = "repl-bata";
    lintel_reverse(filelist,detid,5,weight,strategy,id);
    add_lintel_outline(c1);
    */
    return 0;
}
