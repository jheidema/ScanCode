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
    TH1F *h_nu = new TH1F("hnu","hnu",100,0,10);
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
    vector< double > hbins = tl.GetBins();
    if(hbins.size()!=hfvec.size()) {cout << "Size Error\n"; return;}
    const int nbins = hbins.size();
    float barray[nbins];

    for (int ib=0; ib<nbins; ib++)
        barray[ib] = float(hbins.at(ib));

    if(id<0 || id >1) id = 1;
    //TH1F *h_nu = new TH1F("hnu","hnu",100,0,10);
    TH1F *h_nu = new TH1F("hnu","hnu",nbins-1,barray);
    for (int i=0;i<hfvec.size()-1;i++){
        if(hfvec.at(i).at(id)>0.00001) h_nu->SetBinContent(i,hfvec.at(i).at(id));
    }
    h_nu->SetTitle(filename);
    h_nu->Draw();
    return;
}