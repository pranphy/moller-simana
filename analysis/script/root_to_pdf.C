#include "utils.hh"

void root_to_pdf(std::string filename,std::string directory,std::string opfile){
    //dark_mode();
    TFile f(filename.c_str());
    TDirectory* dir = f.GetDirectory(directory.c_str());
    auto *list = dir->GetListOfKeys();
    TKey *key; TIter iter(list); //or TIter iter(list->MakeIterator());

    TCanvas* canvas = new TCanvas("canvas", "Canvas for histograms", 800, 600);

    canvas->Print((opfile+std::string("[")).c_str(),"pdf");
    while((key = (TKey*)iter())) {
        TObject *hist = key->ReadObj();
        hist->Draw("colz");
        canvas->Print(opfile.c_str(),"pdf");
        std::string name{hist->GetName()};
    }
    canvas->Print((opfile+std::string("]")).c_str(),"pdf");
}
