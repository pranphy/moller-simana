#include <iostream>

#include "TFile.h"
#include "TTreeReader.h"
#include "utils.hh"

//#include "remolltypes.hh"
//#include "remollGenericDetectorHit.hh"
//
typedef std::vector<float> ratevec;
typedef std::vector<hit_list> hitveclist;
struct filestat {
  unsigned long evtread = 0;
  size_t empty = 0;
  size_t hitsum = 0;
  size_t tot911 = 0;
  size_t tot28 = 0;
  size_t nemynon = 0;
  void printhead(){
    printf("evtread,empty,nonempty,hitsum,tot911,total28,nemynon\n");
  }
  void print(){
    printf("%lu, %zu, %lu, %zu, %zu, %zu, %zu \n",evtread,empty,evtread-empty,hitsum,tot911,tot28, nemynon);
  }
  filestat operator+(filestat &n){
    filestat nfs = filestat();
    nfs.evtread = evtread + n.evtread;
    nfs.empty = empty + n.empty;
    nfs.hitsum = hitsum + n.hitsum;
    nfs.tot911 = tot911 + n.tot911;
    nfs.tot28= tot28+ n.tot28;
    nfs.nemynon = nemynon + nemynon;
    return nfs;
  }
};

filestat get_file_stat(std::string filename,bool verbose = false)
{
  //std::cout<<"The filename is "<<filename<<std::endl;
  TFile file(filename.c_str());
  TTreeReader reader("T",&file);
  TTreeReaderValue<hit_list>  hits(reader,"hit");
  TTreeReaderValue<double>  rates(reader,"rate");
  filestat fs;

  if (verbose) printf("Cnt     Rate     len 911 28  elems \n");
  while(reader.Next()) //for(int j = 0; j<= evtcount; ++j)
  {
    size_t nc = std::count_if(hits->begin(),hits->end(), [](RemollHit p){ return p.det == 911;});
    size_t dc = std::count_if(hits->begin(),hits->end(), [](RemollHit p){ return p.det == 28;});
    fs.hitsum += hits->size(); fs.tot911 += nc; fs.tot28 +=dc;
    if (hits->size() == 0) fs.empty += 1; // empty  hitlist
    else if (nc == 0 )  fs.nemynon += 1; // no 911 on nonempty hitlist

    if (verbose){
    printf("%06lu. %f, %zu,  %zu, %zu,  [",fs.evtread,*rates,hits->size(),nc,dc);
    for_each(hits->begin(),hits->end(), [](RemollHit p){std::cout<<p.det<<", ";});
    printf("]\n");
    }
    ++fs.evtread;
  }

  if (verbose) fs.print();
  return fs;
}



void inspect_hit_array(std::string filelist)
{
  std::ifstream infile(filelist);
  std::string filename;
  filestat all;
  std::cout<<"Filename,";
  all.printhead();
  while (std::getline(infile, filename))
  {
    std::cout<<filename<<",";
    filestat fs = get_file_stat(filename);
    all = all + fs;
    fs.print();
  }
  std::cout<<"---------------------------------------------------------------------"<<std::endl;
  std::cout<<"Total,";
  all.print();
}

