#include <stdio.h>

#include "source/HFReader.C"

#include "TGraph.h"

TH1F* HFHist (const char* filename, int id=1){
 delete gROOT->FindObject("hnu");
    HFReader tl;
    tl.SetFileName(filename);
    tl.Import();
    vector< vector< double > > hfvec =  tl.GetVectors();
    if(id<0 || id >1) id = 1;
    TH1F *h_nu = new TH1F("hnu","hnu",70,0,7);
    for (int i=0;i<hfvec.size();i++){
        h_nu->SetBinContent(i,hfvec.at(i).at(id));
    }
    h_nu->SetTitle(filename);
 return h_nu;
}

TGraph* HFGraph (const char* filename, int id=1){

    HFReader tl;
    tl.SetFileName(filename);
    tl.Import();
    vector< vector< double > > hfvec =  tl.GetVectors();
    if(id<0 || id >1) id = 1;
    TGraph *h_nu = new TGraph();
    for (int i=0;i<hfvec.size();i++){
        h_nu->SetPoint(i,(i+1)/10.,hfvec.at(i).at(id));
    }
    h_nu->SetTitle(filename);
return h_nu;
}

void PlotHF(const char* filename, int id=1){
 delete gROOT->FindObject("hnu");
    HFReader tl;
    tl.SetFileName(filename);
    tl.Import();
    vector< vector< double > > hfvec =  tl.GetVectors();
    if(id<0 || id >1) id = 1;
    TH1F *h_nu = new TH1F("hnu","hnu",70,0,7);
    for (int i=0;i<hfvec.size();i++){
        h_nu->SetBinContent(i,hfvec.at(i).at(id));
    }
    h_nu->SetTitle(filename);
 h_nu->Draw();
}