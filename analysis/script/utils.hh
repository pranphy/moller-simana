#pragma once

/**
  * This is `utils.hh` and has a lot of utility functions that I use very often.
  * The stuff here are used by a lot of other scripts in analysis of the simulation
  * data
  */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <functional>


#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TH2D.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TArc.h"

/** The type of `hit` object stored in the remoll simulation root file is 
    `remollGenericDetectorHit_t` which is a bit lot to remember and type. So I 
    typedefed this to RemollHit which is easy to remember

    All the remoll types are defined in remolltypes.hh file.
*/
typedef remollGenericDetectorHit_t RemollHit;

/** 
  Frequently we need a `std::vector` of \ref RemollHit. This is just an
  alias to std::vector<RemollHit> which in turn is an alias so it effect
  this is  std::vector<remollGenericDetectorHit_t> which would be a handful
  to type. So you could just use `hit_list`. `hit_vector` would've been better name.
*/
typedef std::vector<RemollHit> hit_list;
typedef std::function<bool(RemollHit)> hitfunc;
typedef std::function<bool(RemollHit)> hit_cut;



/** The type of `ipart` object stored in the remoll simulation root file is 
    `remollEventParticle_t` which is a bit lot to remember and type. So I 
    typedefed this to  @ref `RemollPart` which is easy to remember

    All the remoll types are defined in remolltypes.hh file.
*/
typedef remollEventParticle_t RemollPart;
typedef std::vector<RemollPart> part_list;

typedef std::function<std::pair<double,double>(RemollHit)> pair_func;

typedef std::tuple<hit_list,part_list,double> RemollHitPartRate;

/*
template<typename Function, typename... Arguments>
auto curry(Function function, Arguments... args) {
    return [=](auto... rest) {
        return function(args..., rest...);
    }; // don't forget semicolumn
}
*/
namespace PID {
    const int ELECTRON =  11;
    const int POSITRON = -11;
    const int PHOTON   =  22;
    const int PROTON   =  2212;
}

namespace DETID {
    const int MD = 28;
}

namespace utl{

// C++ Helpers
/**
  Given a std::string, split it by the delimiter and return the vector of tokens.
  \param full the full string to split
  \param delimiter the delimiter to split the full string with
  \return `std::vector<std::string>` of each token after splitting by delimiter.
*/
std::vector<std::string> split(std::string full, char delimiter=','){
    std::string segment; std::stringstream fullstream(full);
    std::vector<std::string> seglist;

    while(std::getline(fullstream, segment, delimiter)) {
        seglist.push_back(segment);
    }
    return seglist;
}

std::string get_extension(std::string str){
    auto npos = str.find_last_of('.');
    return npos == str.npos ? "" : str.substr(npos+1,str.size());
}



/**
  A handy shortcut function to check if the element elem is inside a container ( a std::vector ).
  \param container a std::vector of any type
  \param elem an object of the sme type as the vector objects.
  \returns true of elem is contained in container else false.
*/
template <typename T>
bool contains(std::vector<T>& container,const T& elem) {
    return container.end() != std::find(container.begin(),container.end(),elem);
}


template <typename T>
bool is_inside_pgm(const T& x, const T& y,std::vector<T> box){
    T x0 = box[0], x1 = box[4], y0 = box[2],  y1 = box[6],
      x2 = box[1], x3 = box[5], y2 = box[3],  y3 = box[7];
    bool L0up   =  (y-y0) > ( (y1-y0)/(x1-x0)*(x-x0) );
    bool L1down =  (y-y0) < ( (y2-y1)/(x2-x1)*(x-x1) );
    bool L2down =  (y-y2) < ( (y2-y3)/(x2-x3)*(x-x3) );
    bool L3up   =  (y-y3) > ( (y3-y0)/(x3-x0)*(x-x0) );
    return L0up and L1down and L2down and L3up;
}


template <typename T>
bool is_inside(const T& x, const T& y,std::vector<T> box){
    if(box.size() <= 4)
        return (x > box[0] and x <= box[1] and y > box[2] and y <= box[3]); //box = [x0,x1, y0, y1]
    else
        return is_inside_pgm(x,y, box);
}


template <typename T>
void print(std::vector<T> container, std::string formatter="%8.1f, "){
    printf("[ ");
    std::for_each(container.begin(),container.end(),[&](T c){ printf(formatter.c_str(),c);});
    printf("] ");
}

template <typename T>
void println(std::vector<T> container, std::string formatter="%8.1f, "){
    print(container,formatter);
    printf("\n");
}

// General utilities

double hypot(double x, double y) { return sqrt(x*x + y*y); };
double to_degree(double a) { return a*180/3.141592653589;}
//! Given a angle a in degrees, returns the radian value of the angle in radians
double to_radian(double a) { return a*3.141592653589/180;}

// inverse tangent in degrees
double atan_deg(double y, double x){
    double rad = atan(y/x);
    double deg = rad*180/3.141592653589;
    if( x <0 and y > 0) deg +=  180;
    if( x <0 and y < 0) deg -=  180;
    return deg;
}



// ROOT Stuffs

/**
  This function saves  objects into rootfile inside a directory. The file is created if it doesn't exist.
    \param Obj (usually histograms right?)
    \param name Name of the object in the root file
    \param directory the directory in the rootfile where the object is saved
    \param rootfile the path to root file to save the object to
*/
void save(TObject* Obj, std::string name, std::string directory="", std::string rootfile="asset/files/saved-root-objects.root"){
    auto F = std::make_unique<TFile>(rootfile.c_str(),"UPDATE");
    if(directory != ""){
        TDirectory* objdir = F->GetDirectory(directory.c_str());
        if(!objdir) objdir = F->mkdir(directory.c_str());
        F->cd(directory.c_str());
    }
    Obj->Write(name.c_str());
    F->Close();
}

/**
  This function saves  std::vector into rootfile inside a directory. The file is created if it doesn't exist.
    \param treename the name of the tree to save data to. Usually this is not very important
    \param data a std::vector any type. usually std::vector<float>
    \param name Name of the object in the root file
    \param directory the directory in the rootfile where the object is saved
    \param rootfile the path to root file to save the object to
*/
template<typename T> void save(std::string treename, std::vector<T> data, std::string name, std::string directory="", std::string rootfile="asset/files/saved-root-data.root"){
    auto F = std::make_unique<TFile>(rootfile.c_str(),"UPDATE");
    if(directory != ""){
        TDirectory* objdir = F->GetDirectory(directory.c_str());
        if(!objdir) objdir = F->mkdir(directory.c_str());
        F->cd(directory.c_str());
    }

    TTree* tree;
    if (F->Get(treename.c_str()) == nullptr) tree = new TTree(treename.c_str(), "Data Tree");
    else tree = dynamic_cast<TTree*>(F->Get(treename.c_str()));

    T elem;
    tree->Branch(name.c_str(), &elem);
    F->cd();
    for(auto dt: data) {elem = dt; tree->Fill(); }
    F->Write();
    F->Close();
}

//! Same as other data save function but omits the treename parameter to set to "T", imitating the remoll tree name.
template<typename T>
void save(std::vector<T> data, std::string name, std::string directory="", std::string rootfile="asset/files/saved-root-objects.root"){
    save("T",data,name,directory,rootfile);
}


/// Graphics stuffs
void draw_line(std::vector<double> p0, std::vector<double> p1){
    TLine* line = new TLine(p0[0],p0[1], p1[0],p1[1]);
    line->SetLineColor(kBlue); line->SetLineColor(2); line->SetLineWidth(1);
    line->Draw();
}

/**
  Adds a vertical line on the screen. The colour is red.
  \param xval The x coordinate at the current axis where the vertical line is drawn
*/
void add_vline(double xval){
    TLine* line = new TLine(xval, gPad->GetUymin(), xval, gPad->GetUymax());
    line->SetLineColor(kRed);
    line->SetLineStyle(2);
    line->SetLineWidth(1);
    line->Draw();
}

/**
  Adds a horizontal line on the screen. The colour is red.
  \param yval The y coordinate at the current axis where the horizontal line is drawn
*/
void add_hline(double yval){
    TLine* line = new TLine( gPad->GetUxmin(), yval, gPad->GetUxmax(),yval);
    line->SetLineColor(kRed);
    line->SetLineStyle(2);
    line->SetLineWidth(1);
    line->Draw();
}

/**
  Draws a circle of given radius, and center (optionally)
  \param radius The radius of the circle to draw
  \param x The x coordinate of the center
  \param y The y coordinate of the center
*/
void draw_circle(float radius, float x = 0, float y = 0){
    TArc* circle = new TArc(x, y, radius);
    circle->SetLineColor(kRed); circle->SetLineStyle(2); circle->SetLineWidth(1);
    circle->SetFillStyle(4001);
    circle->Draw();
}


template<typename T>
void draw_polygon(std::vector<T>& x, std::vector<T>& y,int colour=kRed, int thickness=2,int style=2){
    TPolyLine* poly = new TPolyLine(x.size(),x.data(),y.data());
    poly->SetLineWidth(thickness); poly->SetLineColor(colour); poly->SetLineStyle(style); poly->Draw();
}

template<typename T>
void draw_quad(std::vector<T>& box,int colour=kRed, int thickness=2,int style=2){
    std::vector<T> x={box[0],box[0],box[1],box[1],box[0]};
    std::vector<T> y={box[2],box[3],box[3],box[2],box[2],};
    draw_polygon(x, y, colour, thickness, style);
}


template<typename T>
void make_rectangle(std::vector<T>& box,int colour=kRed, int thickness=2,int style=2){
    std::vector<float> x={box[0],box[0],box[1],box[1],box[0]};
    std::vector<float> y={box[2],box[3],box[3],box[2],box[2],};
    draw_polygon(x, y, colour, thickness, style);
}

void show_text(std::string text, Double_t normx, Double_t normy, int colour=kRed, Double_t size = 0.04, int NDC=0) {
  TLatex* label = new TLatex();
  label->SetNDC(NDC);
  label->SetTextAlign(22);
  label->SetTextSize(size);
  label->SetTextColor(colour);
  label->DrawLatex(normx, normy, text.c_str());
}

// Remoll/Moller Specific

/**
  For a given hit, this function returns true if that hit falls on 
  moller detector (DETID::MD=28) and on ring given as the second parameter.
  The bounds of ring are taken from [DocDB 896](https://moller.jlab.org/cgi-bin/DocDB/private/ShowDocument?docid=896)
  There is a offset parameter here, which I was told was the updated ring values.
  \param hit The RemollHit object
  \param ring The ring number of moller detector. 1-6 means the corresponding ring. 0 means all the rings.
  \returns true if it falls on given ring in det 28 false if it doesn't.
*/
bool md_ring_cut(RemollHit hit, int ring = 0){
    int rmin = 0; int rmax = 0; int offset = 10; // Recheck offset
    if(ring == 0){ rmin = 650; rmax = 1160; } // ring = 0 means no ring cut
    else if(ring == 1) {rmin = 650; rmax = 680;} // Table from  DocDB 896-v1
    else if(ring == 2) {rmin = 680; rmax = 740; }
    else if(ring == 3) {rmin = 740; rmax = 800; }
    else if(ring == 4) {rmin = 800; rmax = 920; }
    else if(ring == 5) {rmin = 920; rmax = 1060; }
    else if(ring == 6) {rmin = 1060; rmax = 1160; }
    return (hit.det == DETID::MD) && (hit.r > rmin+offset) && (hit.r <= rmax+offset);
}

/** (e) (p)lus/(m)inus cut; only exists for backward compatibility \ref cut::epm
  \param hit The Remollhit object
  \returns true if the hit is a particle with pid \f$ \pm 11\f$  otherwise false
*/
bool epm_cut(RemollHit hit){
    return hit.pid == PID::ELECTRON || hit.pid == PID::POSITRON;
}

bool photon_cut(RemollHit hit){ return hit.pid == PID::PHOTON; }


namespace cut{
    bool md_ring(RemollHit hit,int ring)  { return md_ring_cut(hit,ring); };
    bool E1(RemollHit hit) { return hit.e > 1; }
    //! Returns true if the hit pid is electron or positron.
    bool epm(RemollHit hit) { return epm_cut(hit); } 
    bool ring5_epm(RemollHit hit){ return epm_cut(hit) and md_ring_cut(hit,5); }
    bool ring5_epm_E1(RemollHit hit){ return ring5_epm(hit) && E1(hit); }
    bool ring0_epm_E1(RemollHit hit){ return md_ring(hit,0) && epm(hit) && E1(hit); }
    //! Returns true if the hit pid is photon
    bool photon(RemollHit hit) {return photon_cut(hit); }
    //! Returns true if it hits ring 5 of Main detector and has energy > 1MeV and also is a photon.
    bool ring5_photon_E1(RemollHit hit) {return photon_cut(hit) and md_ring_cut(hit,5) and E1(hit); }
    auto det = [](int det){ return [det](RemollHit hit)->bool { return hit.det == det; }; };

    hit_cut r(float r1,float r2=3500){
        return [r1,r2](RemollHit hit) { return hit.r > r1 and hit.r <= r2; };
    };

    hit_cut E(float E1, float E2 =  12000){
        return [E1,E2](RemollHit hit) { return hit.e > E1 and hit.e <= E2; };
    };

} // cut::

bool __pass(RemollHit) { return true; }

typedef std::function<double(RemollHit)> __remhit_attrib_d;
auto __attrib_trid = [](RemollHit hit)->double { return hit.trid; };

/**
  Takes a vector of hit_cut and returns a hit_cut such that it
  only evaluates to true if each of the cut in the vector evaluates
  to true
  \param cuts std::vector<hit_cut>, the list of cuts to and
  \return lambda that evaluates to true if all the cuts pass when passed hit.
*/
auto all(std::vector<hit_cut>&& cuts){
    return [cuts](RemollHit hit) ->bool {
        bool anded = true;
        for(auto cut: cuts) anded = cut(hit) and anded;
        return anded;
    };
}


/**
  Given hit array, this function looks first for hits that pass the first cut `cut_s`. For those hits,
  it looks again in the hit array to select those hits that pass the second cut `cut_d` and also have
  the same trackid as the first set of hit.
  \param hits the array of hits of type std::vector<RemollHit> which is what is saved in hit branch of remoll tree.
  \param cut_s The source cut;
  \param cut_d The destination cut (default value __pass) which returns true
  \param param The parameter of the hit object to look for, trid for trackid and mtrid for mother track
  */
hit_list select_tracks(hit_list hits, std::function<bool(RemollHit)> cut_s, std::function<bool(RemollHit)> cut_d = __pass, __remhit_attrib_d param=__attrib_trid){
    std::vector<int> trids;
    std::vector<RemollHit>  rev_hits;
    for(auto hit: hits) if(cut_s(hit)) trids.push_back(param(hit));
    for(auto hit: hits) if(contains(trids,hit.trid) and cut_d(hit)) rev_hits.push_back(hit);
    return rev_hits;
}

/**
  Same implementation as \ref select_tracks but looks for mtrid for mother tracks instead for trid for tracks.
  */
hit_list select_mother_tracks(hit_list hits, std::function<bool(RemollHit)> cut_s, std::function<bool(RemollHit)> cut_d = __pass){
    __remhit_attrib_d param = [](RemollHit hit)->double { return hit.mtrid; };
    return select_tracks(hits,cut_s,cut_d,param);
}


bool __get_det_hit(hit_list hits, int trid, hit_cut cut, RemollHit& dethit){
    for(auto hit : hits) if(cut(hit) and hit.trid == trid) { dethit = hit; return true; }
    return false;
}

/**
   Given a list of cuts and a list of hits, looks up those specific tracks that pass all the cuts.
   It returns a vector of all those hit_list where each hit_list is the hit corresponding to each
   cut in the cut list. 
   \param hits The hits to look up from, this is typically for an event
   \param cuts The list of cut function, to select those hits that pass
   \returns a matrix of hits corresponding to the cuts
*/
std::vector<hit_list> lookup_tracks(hit_list hits, std::vector<hit_cut> cuts){
    std::set<int> trids; for(auto hit: hits) if(cuts[0](hit)) trids.insert(hit.trid);
    std::vector<hit_list> trackhits;
    for(int trid : trids){
        bool all = true; hit_list cuthits;
        for(auto cut : cuts){
            RemollHit tmphit;
            all = all and __get_det_hit(hits, trid, cut, tmphit);
            cuthits.push_back(tmphit);
        }
        if(all) trackhits.push_back(cuthits);
    }
    return trackhits;
}

/**
  This function takes 2 detector ids, (and a basecut) and returns a lambda.
  The lambda will take a hit_list and a TH2* pointer. Now it will loop through the
  hits fills the histogram with hit.r at det1 vs hit.r at det2 with the same trackid
  and passing the cut passed as the third parameter.
  \param det1 The detid of first detector
  \param det2 The detid of second detector
  \param basecut The cut that each of the tracks has to pass (usually utl::cut::epm).
  \returns lambda(hit_list, TH2*) where TH2* will be filled with hit.r at det1 vs hit.r at det2
*/
auto rr_correlate(int det1, int det2, hit_cut basecut = utl::cut::epm){
    return [basecut,det1,det2](hit_list& hl, TH2D* hist)->void{
        std::vector<hit_cut> cuts{basecut};
        std::vector<int> dets = {det1,det2}; for(auto detid : dets) cuts.push_back(cut::det(detid));
        std::vector<hit_list> those_tracks = lookup_tracks(hl, cuts);
        for(hit_list& cuthits : those_tracks){
            RemollHit h0 = cuthits[1], h1 = cuthits[2];
            hist->Fill(h0.r, h1.r);
        }
    };
}




template <typename T>
bool det_pid_cut(T obj,int det, int pid) {
    return obj.det == det && (obj.pid == pid || obj.pid == -pid);
}

// Remoll Data
/**
  Returns a TTree object for the given filename. It looks for a tree named "T" (the default tree name for remoll(
  doesn't care if the the filename points to a zombie file. It doesn't make sure that the file exists
  so have to pase a existing tree file.
  \param filename The path to the root file.
  \param treename The name of the tree to read ("T" by default)
  \returns Pointer to TTree object.
  */
TTree* get_tree(std::string filename,const std::string treename="T"){
    TFile* F = new TFile(filename.c_str());
    TTree* T = F->Get<TTree>(treename.c_str());
    return T;
}

/**
  For a given filename checks if the root file is a zombie file. If it is a zombie file
  it will return false. Otherwie it will update the reference to the tree and return true.
  \param filename The path to root file.
  \param T The address of the pointer to a Ttree object which has to be updated.
  \param treename The name of the Tree in the root file ("T" by default)
  returns true if the file is not a zombie, false if it is.

  */
bool get_tree(std::string filename, TTree** T, const std::string treename="T"){
    TFile* F = new TFile(filename.c_str());
    if(F->IsZombie())
        return false;
    else
        *T = F->Get<TTree>(treename.c_str());
    return true;
}

/**
  Reads the text file line by line and returns all lines as a vector of strings.
  \param filename The path of text file to read
  \returns `std::vector<std::string>` of each line in file.
*/
std::vector<std::string> readlines(std::string filename){
    std::vector<std::string> all_lines;
    std::string line; std::ifstream fileobj(filename);
    while(std::getline(fileobj,line)){
        all_lines.push_back(std::move(line));
    }
    return all_lines;
}

TChain* make_chain(std::string filelist,int nof = 0, bool verbose=false){
    std::ifstream flist(filelist);
    std::string filename;
    TChain* TC = new TChain("T");
    int cnt = 0;
    while(std::getline(flist,filename)){
        if(++cnt > nof && nof > 0)  break;
        if(filename.length()<2) continue;
        if(verbose) std::cout<<"Adding "<<filename<<std::endl;
        TC->Add(filename.c_str());
    }
    return TC;
}


template <typename T>
bool has_pid(std::vector<T> hits, int pid) {
    return hits.end() != std::find_if(hits.begin(),hits.end(),[&](RemollHit hit)->bool{ return hit.pid == pid;});
}

bool has_electron(hit_list hits) { return has_pid(hits,PID::ELECTRON) || has_pid(hits,PID::POSITRON); }
bool has_proton(hit_list hits) { return has_pid(hits,PID::PROTON) ; }

// Moller Specific |> calculations
/**
  Assuming that there are 1 Moller events per 3300 beam events,
  this function calculates the rate of moller electron in the detector
  if we provide the number of electron detected for a given number of primary 
  simulation. The factor 1/3000 is for the Moller Detector, so it makes sense to nly
  pass the number of electron detected in the MD.
  \param count The number of electron detected in the Moller Detector (28)
  \param primary The number of primary events simulated.
  \returns the moller rate as a decimal number.
  */
float moller_fraction(float count, float primary){
    float moller_per_beam = 1.0/3300.0; // we expect 1 moller per 3300 beam events.
    float moller_number = moller_per_beam * primary;
    return count/moller_number;
}

/**
  Basically same as \ref moller_fraction function but also accepts the number of secondary simulated 
  and the number of skim events used to run the secondary.
  \param count The number or flectrons detected in the MD
  \param secondary The number of events simulated for the secondary
  \param skim The number of events in the skim file used as generators for secondary.
  \param primary the number of primary events simulated, from which the skim file is produced
  \returns The moller rate as a decimal number
  */
float moller_fraction(float count, float secondary,float skim, float primary){
    float countdash = count/(secondary/skim);
    return moller_fraction(countdash,primary);
}

} // utl::

