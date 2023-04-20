#include <map>

#include "remolltypes.hh"
#include "RemollTree.hpp"

namespace msc {

std::map<int,float> detloc = {
    {817,  319.00      },
    {44 ,  3224-0.5    },
    {45 ,  3375+0.5    },
    {48 ,  4707.5-0.5  },
    {49 ,  4809.1+0.6  },
    {58 ,  7557.82-0.5 },
    {60 ,  7770.54+0.5 },
    {61 ,  8150.5      },
    {62 ,  8500-0.5    },
    {63 ,  8800+0.5    },
    {64 ,  9200+0.5    },
    {65 ,  9643.99+0.5 },
    {66 ,  9555.05     },
    {67 ,  9749.99+0.5 },
    {69 ,  9708.80     },
    {72 ,  10927.50    },
    {74 ,  11080.404   },
    {76 ,  11814.49    },
    {77 ,  11967.50    },
    {80 ,  19132.69    },
    {81 ,  19282.75    },
    {28 ,  22200       },
    {815,  14500       },
    {816,  25170       },
};

struct component {
    std::string name;
    int detid;
    std::vector<float> box;
};

namespace cmp {
    const auto Coll6A = component{"Collimator 6A", 66, {9555.05, 9555.05 +69.85 + 82.55, 50,80}};
    const auto Coll6B = component{"Collimator 6B", 72, {10927, 10927+69.85+82.55, 90,120}};
}


namespace  cut_s{
struct cut{
    std::string cutstr;
    std::string str() { return cutstr; }
    cut operator+(std::string pcutstr){ cutstr += " && " + pcutstr; return *this; };
    cut operator+(cut& ct){ cutstr += " && " + ct.cutstr; return *this; };
};
cut ring5  = cut{" (hit.det == 28 && hit.r > 930 && hit.r <= 1070) "};
cut epm    = cut{" (hit.det == 11 || hit.pid == -11) "};
cut photon = cut{" (hit.pid == 22 ) "};
cut col6a  = cut{" hit.det == 66 "};
cut col6b  = cut{" hit.det == 72 "};
cut trid1 = cut{" hit.trid == 1 "};
cut tridnot1 = cut{" hit.trid != 1 "};
cut ring5_epm = ring5 + epm;
cut ring5_photon = ring5 + photon;
}


std::vector< std::vector<int> > get_counts(RemollTree& RT, std::vector<hit_cut>& cuts, std::vector<double>flpartition={-1e5,-3700,8500,14500,22200, 35000,1e6}){
    std::vector< std::vector<int> > count_cuts;
    for(size_t i = 1; i < flpartition.size(); ++i){
        std::vector<int> counts(cuts.size(),0);
        double minvz = flpartition[i-1], maxvz = flpartition[i];
        auto fill_count = [&](RemollHit hit){
            for(size_t j = 0; j < cuts.size(); ++j) if(cuts[j](hit) and hit.vz > minvz and  hit.vz <= maxvz) ++counts[j]; 
        };
        loop_tree(RT,fill_count);
        count_cuts.push_back(counts);
    }
    return count_cuts;
}



std::vector<int> get_counts(RemollTree& RT,hit_cut cut ,std::vector<double>flpartition={-1e5,-3700,8500,14500,22200, 35000,1e6}){
    std::vector<int> counts;
    //float total = 2029;
    for(size_t i = 1; i < flpartition.size(); ++i){
        double minvz = flpartition[i-1], maxvz = flpartition[i];
        int count = 0; auto fill_count = [&](RemollHit hit){ if(cut(hit) and hit.vz > minvz and hit.vz <= maxvz) ++count; };
        loop_tree(RT,fill_count);
        counts.push_back(count);
    }
    return counts;
}



std::vector<int> get_counts(TTree* T,std::string cut="",std::vector<double>flpartition={-1e5,-3700,8500,14500,22200, 35000,1e6}){
    std::vector<int> counts;
    std::string cstr = cut == "" ? "(hit.pid == 11 || hit.pid == -11) && hit.det == 28 && hit.e > 1 && hit.r > 930 && hit.r <= 1070" : cut;
    //float total = 2029;
    for(size_t i = 0; i < flpartition.size()-1; ++i){
        int count = T->Draw("hit.vz",Form("%s && hit.vz > %f && hit.vz <= %f",cstr.c_str(),flpartition[i],flpartition[i+1]),"groff");
        counts.push_back(count);
    }
    return counts;
}

void print_counts(std::vector<int> counts,float primary=1e8 ,std::vector<double>flpartition={-1e5,-3700,8500,14500,22200, 35000,1e6}){
    float total = 0; for(int count: counts) total += count;
    printf(" -------------------------------------------------------------\n");
    printf(" (vzmin           vzmax]    count         ratio  (moller rate) \n");
    printf(" -------------------------------------------------------------\n");
    for(size_t i = 1; i < flpartition.size(); ++i){
        int count = counts[i-1];
        float mf = utl::moller_fraction(count,primary)*100;
        printf("%11.2f  %11.2f : %5d:      %8.3f  (%0.3f%%) \n",flpartition[i-1],flpartition[i],count,count/total,mf);
    }
    float mf = utl::moller_fraction(total,primary)*100;
    printf("--------------------------------------------------------------\n");
    printf("%11.2f %11.2f : %5.0f:      %8.3f   (%0.3f%%) \n",flpartition[0],flpartition[flpartition.size()-1],total,total/total,mf);
}

void print_counts(std::vector<std::vector<int>> count_cuts,std::vector<std::string>& titles,float primary=1e8 ,std::vector<double>flpartition={-1e5,-3700,8500,14500,22200, 35000,1e6}){
    std::vector<int> totv;
    int lng  = 40 + 25*titles.size();
    for(size_t i = 0; i < count_cuts[0].size(); ++i){
        int total = 0; for(size_t j = 1; j < flpartition.size(); ++j) total += count_cuts[j-1][i]; 
        totv.push_back(total);
    }
    printf("%s\n",std::string(lng,'-').c_str());
    printf("                           ");
    for(auto title : titles) printf("          %s          ",title.c_str());
    printf(" \n (vzmin           vzmax]   ");
    for(auto title : titles) printf(" count    ratio (moller %%) ");
    printf("\n");
    printf("%s\n",std::string(lng,'-').c_str());
    for(size_t i = 1; i < flpartition.size(); ++i){
        std::vector<int> counts = count_cuts[i-1];
        printf("%11.2f  %11.2f : ",flpartition[i-1],flpartition[i]);
        for(size_t j = 0; j < counts.size(); ++j){
            float total = totv[j];
            float mf = utl::moller_fraction(counts[j],primary)*100;
            printf(" %5d %8.3f (%0.3f%%)   ",counts[j],counts[j]/total,mf);
        }
        printf("\n");
    }
    printf("%s\n",std::string(lng,'-').c_str());
    printf("%11.2f   %11.2f: ",flpartition[0],flpartition[flpartition.size()-1]);
    for(int tot : totv) printf(" %5d %8.3f (%.3f%%)   ",tot,float(tot)/tot,utl::moller_fraction(tot,primary)*100);
    printf("\n");
}

// very lame cut bank
//const std::map<std::string, hit_cut> cut_bank = {
//    {"ring5epm", utl::ring5_epm},
//    {"epm", utl::epm_cut},
//    {"epmfrom6a", [](RemollHit hit)->bool{
//                       double vr = utl::hypot(hit.vx,hit.vy);
//                       return utl::epm_cut(hit) and vr > 64 and vr < 85
//                           and hit.vz > detloc[66] - 10, hit.vz < detloc[66] + 10;
//                   }
//    },
//};

// stuffs that c++17 supports and apparantly doesn't work in ifarm because it has c++14 only.

auto track_selector(hit_cut cut){
    return [cut](hit_list hits) { return utl::select_tracks(hits,cut); };
}

auto mother_track_selector(hit_cut cut){
    return [cut](hit_list hits) { return utl::select_mother_tracks(hits,cut); };
}


template<typename T=RemollHit>
std::function<bool(T)> detid_cut(int det){
    return [det](T obj)->bool { return obj.det == det; };
}

template<typename T=RemollHit>
auto trid_cut(int trid){
    return [trid](T obj)->bool { return obj.trid == trid; };
}

template<typename T=RemollHit>
auto pid_cut(int pid){
    return [pid](T obj)->bool { return obj.pid == pid; };
}

auto all(std::vector<hit_cut> cuts){
    return [cuts](RemollHit hit) ->bool {
        bool anded = true;
        for(auto cut: cuts) anded = cut(hit) and anded;
        return anded;
    };
}


} // msc::

namespace hst {


typedef std::function<double(RemollHit)> __attrib_f;

TH1D* hist1d(RemollTree& RT, hit_cut cut,
        __attrib_f param,
        std::vector<float> w,
        int nbins=100,
        std::string name="1dhist",
        std::string title="1dhist; x; y",
        lookup_func lookup = identity_lookup) {
    auto hist = new TH1D(name.c_str(),title.c_str(),nbins,w[0],w[1]);
    auto fill_h = [&](RemollHit hit){
        double x = param(hit);
        if( cut(hit)  and x > w[0] and x <= w[1] ) hist->Fill(x);
    };
    loop_tree(RT,fill_h,lookup);
    return hist;
}

TH1D* e(RemollTree& RT, lookup_func lookup, hit_cut cut, std::vector<float> w,int nbins=100, std::string title="Energy; E[MeV]; Count"){
    auto param = [&](RemollHit hit)->double { return hit.e; };
    return hist1d(RT,cut,param,w,nbins,"ehist",title,lookup);
}


TH1D* e(RemollTree& RT, hit_cut cut, std::vector<float> w,int nbins=100, std::string title="Energy; E[MeV]; Count"){
    return e(RT,identity_lookup,cut,w,nbins,title);
}


TH1D* e(RemollTree& RT, std::vector<float> w,int nbins=100, std::string title="Energy; E[MeV]; Count "){
    return e(RT,identity_lookup,utl::__pass,w,nbins,title);
}

TH1D* vz(RemollTree& RT, lookup_func lookup, hit_cut cut, std::vector<float> w,int nbins=100, std::string title="Vertex positions; vz[mm]; Count"){
    auto param = [&](RemollHit hit)->double { return hit.vz; };
    return hist1d(RT,cut,param,w,nbins,"ehist",title,lookup);
}

TH1D* vz(RemollTree& RT, lookup_func lookup, std::vector<float> w,int nbins=100, std::string title="Vertex positions; vz[mm]; Count"){
    return vz(RT,lookup,utl::__pass,w,nbins,title);
}

TH1D* vz(RemollTree& RT, hit_cut  cut, std::vector<float> w,int nbins=100, std::string title="Vertex positions; vz[mm]; Count"){
    return vz(RT,identity_lookup,cut,w,nbins,title);
}


TH1D* vz(RemollTree& RT, std::vector<float> w,int nbins=100, std::string title="Vertex positions; vz[mm]; Count"){
    return vz(RT,identity_lookup,utl::__pass,w,nbins,title);
}


TH2D* hist2d(RemollTree& RT, hit_cut cut,
        __attrib_f first, __attrib_f second,
        std::vector<float> w,
        int xbins=100, int ybins=100,
        std::string name="2dhist",
        std::string title="2dhist; x; y",
        lookup_func lookup=identity_lookup) {
    auto xyhist = new TH2D(name.c_str(),title.c_str(),xbins,w[0],w[1],ybins,w[2],w[3]);
    auto fill_2d = [&](RemollHit hit){
        double x = first(hit), y = second(hit);
        if( cut(hit)  and x > w[0] and x <= w[1] and y > w[2] and y <= w[3] ) xyhist->Fill(x,y);
    };
    loop_tree(RT,fill_2d,lookup);
    return xyhist;
}

TH2D* vzvr(RemollTree& RT, lookup_func lookup, hit_cut cut,std::vector<float> w, int nbins=100, std::string title="vr vs vz"){
    auto first = [](RemollHit hit)->double { return hit.vz; };
    auto second = [](RemollHit hit)->double { return utl::hypot(hit.vx,hit.vy); };
    return hist2d(RT,cut,first,second,w,nbins,nbins,"vzvrhist",title+" ;vz[mm]; vr[mm]",lookup);
}


TH2D* vzvr(RemollTree& RT, lookup_func lookup, std::vector<float> w, int nbins=100, std::string title="vr vs vz"){
    return  vzvr(RT,lookup,utl::__pass,w,nbins,title);
}


TH2D* vzvr(RemollTree& RT, hit_cut cut, std::vector<float> w, int nbins=100, std::string title="vr vs vz"){
    return  vzvr(RT,identity_lookup,cut,w,nbins,title);
}


TH2D* vzvr(RemollTree& RT, std::vector<float> w, int nbins=100, std::string title="vr vs vz"){
    return  vzvr(RT,identity_lookup,utl::__pass,w,nbins,title);
}


TH2D* xy(RemollTree& RT,lookup_func lookup, hit_cut cut, std::vector<float> w, int nbins=100, std::string title="y vs x"){
    auto first = [](RemollHit hit)->double { return hit.x; };
    auto second = [](RemollHit hit)->double { return hit.y; };
    return hist2d(RT,cut,first,second,w,nbins,nbins,"xyhist",title+"; x[mm]; y[mm]",lookup);
}


TH2D* xy(RemollTree& RT, lookup_func lookup, std::vector<float> w, int nbins=100, std::string title="y vs x"){
    return xy(RT,lookup,utl::__pass,w,nbins,title);
}


TH2D* xy(RemollTree& RT, hit_cut cut, std::vector<float> w, int nbins=100, std::string title="y vs x"){
    return xy(RT,identity_lookup,cut,w,nbins,title);
}

TH2D* xy(RemollTree& RT, std::vector<float> w, int nbins=100, std::string title="y vs x"){
    return xy(RT,identity_lookup,utl::__pass,w,nbins,title);
}

TH2D* zr(RemollTree& RT, lookup_func lookup, hit_cut cut, std::vector<float> w, int nbins=100, std::string title="r vs z"){
    auto first = [&](RemollHit hit)->double { return hit.z; };
    auto second = [&](RemollHit hit)->double { return hit.r; };
    return hist2d(RT,cut,first,second,w,nbins,nbins,"zrhist",title+"; z[mm]; r[mm]",lookup);
}


TH2D* zr(RemollTree& RT, lookup_func lookup, std::vector<float> w, int nbins=100, std::string title="r vs z"){
    return zr(RT,lookup,utl::__pass,w,nbins,title);
}

TH2D* zr(RemollTree& RT, hit_cut cut, std::vector<float> w, int nbins=100, std::string title="r vs z"){
    return zr(RT,identity_lookup,cut,w,nbins,title);
}


TH2D* zr(RemollTree& RT, std::vector<float> w, int nbins=100, std::string title="r vs z"){
    return zr(RT,identity_lookup,utl::__pass,w,nbins,title);
}

}; // hist::

