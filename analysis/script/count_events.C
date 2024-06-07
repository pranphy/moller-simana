#include <string>
#include <vector>

#include "utils.hh"


void count_events(std::string filelist){
    std::ifstream flist(filelist); std::string filename;
    int cnt = 0;
    while(std::getline(flist,filename)){
        int count = -1; TTree* T = nullptr;
        if(utl::get_tree(filename,&T)) count = T->GetEntries();
        std::cout<<"COUNTOF "<<++cnt<<", "<<filename<<", "<<count<<std::endl;
    }
}

