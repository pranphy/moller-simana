
#include <iostream>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"


//#int mydet=1006;
const int DETNO=911;

TFile *ofile;
TTree *otree;
TBranch *b_rate,*b_hit;
Double_t newrate=0;
long total_hits=0;
std::vector<remollGenericDetectorHit_t>  *newhit=0;

double scaleRate(1);

long process_one(std::string fnm);
long get_events(std::string);

void SkimTree(std::string input_filename, std::string ofilename, int testRun=0, int beamGen=1){
    long nTotHits(0);
    int nFiles(0);

    if(input_filename==""){
        std::cout<<"\t did not find input file. Quitting!"<<std::endl;
        return;
    }

    std::cout<<"The output filename is :"<<ofilename<<std::endl;
    
    ofile = new TFile(TString(ofilename),"RECREATE");

    otree     = new TTree("T", "skim tree");
    b_rate = otree->Branch("rate", &newrate);
    b_hit   = otree->Branch("hit", &newhit);

    if( input_filename.find(".root") < input_filename.size() ){
        std::cout<<"Processing single file:\n\t"<<input_filename<<std::endl;
        if(beamGen){
            scaleRate = get_events(input_filename);
        }

        nTotHits+=process_one(input_filename);
        nFiles=1;
    }else{
        std::cout<<"Attempting to process list of output from\n\t"<<input_filename<<std::endl;
        std::ifstream ifile(input_filename.c_str());
        std::string data;
        while(ifile>>data){
            nFiles++;
            if(beamGen)
                scaleRate += get_events(data);

            if(testRun==1 && nFiles>10)
                break;
        }
        ifile.close();
        if(!beamGen)
            scaleRate = nFiles;
        nFiles=0;

        std::cout<<"\t scale rate by "<<scaleRate<<std::endl;

        ifile.open(input_filename.c_str());
        while(ifile>>data){
            nFiles++;
            std::cout<<" processing: "<<data<<std::endl;
            nTotHits+=process_one(data);
            if(testRun==1 && nFiles>10)
                break;
        }
    }

    ofile->cd();
    otree->Write();
    ofile->Close();

    std::cout<<"\nFinished processing a total of "<<nTotHits<<" hits."<<std::endl;
}

long get_events(std::string fnm){
    TFile* ifile = new TFile(fnm.c_str(),"READ");
    TTree* itree = (TTree*)ifile->Get("T");
    long evn = itree->GetEntries();
    ifile->Close();
    delete ifile;

    return evn;
}


long process_one(std::string filename){
    std::cout<<"Reading"<<filename;
    
    TFile* ifile = new TFile(filename.c_str(),"READ");
    TTree* itree = (TTree*)ifile->Get("T");

    long nEntries = itree->GetEntries();
    total_hits += nEntries;
    std::cout<<" Found: "<<nEntries<<"events, which makes:"<<total_hits<<std::endl;

    Double_t rate;
    std::vector<remollGenericDetectorHit_t>  *hit=0;
    itree->SetBranchAddress("rate", &rate);
    itree->SetBranchAddress("hit", &hit);

    long nHits(0);
    double tempnum;
    for (long i=0; i < nEntries;i++){
        itree->GetEntry(i);
        tempnum=0;
        for(int j=0; j<hit->size(); j++){
            //if(hit->at(j).det == cur_det && hit->at(j).trid == 1) {
                //make cut to avoid double counting ... actually can just do j=0 
             //   	  if(hit->at(j).px!=hit->at(j-1).px&&hit->at(j).py!=hit->at(j-1).py&&hit->at(j).pz!=hit->at(j-1).pz)
                if(tempnum==0){
                    tempnum++;//count one detector hit of trackid =1 per entry, otherwise double counting
                    newhit->push_back(hit->at(j));	
                    newrate = rate/scaleRate;

                    if(rate == 0)
                        newrate = 1/scaleRate;
                    nHits++;
                }
            //}
        }
        if(newhit->size()>0){
            otree->Fill();
            newhit->clear();
            newrate = 0;
        }
    }
    ifile->Close();
    std::cout<<"Now  returning whith "<<nHits<<std::endl;
    
    delete ifile;

    return nHits;
}

