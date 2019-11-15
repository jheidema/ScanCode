#include "tofFuncClass.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "TH1F.h"
#include "TF1.h"

using namespace std;

vector< pair<Double_t, Double_t> > stInfo;
Double_t mn = 939.57; //MeV/c^2
Double_t c = 2.998E8;

void LoadInput(const char *fileName);


void FitSpectrum(TH1F *hIn, const char *fileName){
    
    LoadInput(fileName);
    Int_t nStates = stInfo.size();
    tofFuncClass tf;
    TF1 *f1 = new TF1("f1",tf,0,250,nStates*3+2);
    f1->FixParameter(0,nStates);
    f1->SetNpx(2000);
    f1->SetLineColor(kBlack);

    for (int iv=0;iv<nStates;iv++){
        f1->FixParameter(iv*3+1,stInfo.at(iv).second);
        f1->SetParameter(iv*3+3,0.2);
        f1->SetParLimits(iv*3+3,0.1,0.5);
    }

    string iFit;
    cout  << "Would you like to fit? (y/n)" << endl;
    getline(cin,iFit);
    TF1 *fS[nStates];
    if (iFit=="y"){
        hIn->Fit(f1,"NQ","",30,250);
        for (int iv=0;iv<nStates;iv++){
        fS[iv] = new TF1(Form("F%d",iv),calcResponse,0,250,3);
        fS[iv]->SetNpx(1000);
        fS[iv]->SetParameters(f1->GetParameter(iv*3+1),f1->GetParameter(iv*3+2),f1->GetParameter(iv*3+3));
        }
    }
    f1->Draw("same");
    for(int i=0; i<nStates; i++) fS[i]->Draw("same");

    return;
}

void LoadInput(const char *fileName){
    ifstream fin(fileName);
    Double_t Ex, Sn, M;
    Int_t A;
    Double_t En, t0;
    
    fin >> A >> M >> Sn;
    cout << A << " " << M << " " << Sn << endl;
    while(true){
        fin >> Ex;
        if(fin.eof()) break;
        En = (Ex-Sn)/1000./(1+mn/M);
        t0 = sqrt(1./2.*mn/En)/c;
        stInfo.push_back(std::make_pair(En,t0*1.0E9));
    }
    //stInfo.pop_back();
    
    fin.close();
    return;
}