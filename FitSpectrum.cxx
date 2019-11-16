#include "tofFuncClass.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "TH1F.h"
#include "TF1.h"

using namespace std;

vector< pair<Double_t, Int_t> > stInfo;
const Double_t mn = 939.57; //MeV/c^2
const Double_t c = 2.998E8;
const int npars = 2;

void LoadInput(const char *fileName){
    ifstream fin(fileName);
    Double_t Ex, Sn, M;
    Int_t A;
    Double_t En, t0;
    Int_t kFit;

    fin >> A >> M >> Sn;
    cout << A << " " << M << " " << Sn << endl;
    while(true){
        fin >> Ex >> kFit;
        if(fin.eof()) break;
        En = (Ex-Sn)/1000./(1+mn/M);
        stInfo.push_back(std::make_pair(En,kFit));
    }
    //stInfo.pop_back();
    
    fin.close();
    return;
}

void FitSpectrum(TH1F *hIn, const char *fileName){
    
    LoadInput(fileName);
    Int_t nStates = stInfo.size();
    tofFuncClass tf;
    TF1 *f1 = new TF1("f1",tf,0,250,nStates*npars+1);
    f1->FixParameter(0,nStates);
    f1->SetNpx(2000);
    f1->SetLineColor(kBlack);

    hIn->GetListOfFunctions()->Clear();

    Double_t t0;
    
    for (int iv=0;iv<nStates;iv++){
        Double_t en = stInfo.at(iv).first;
        t0 = sqrt(1./2.*mn/en)/c*1.0E9;
        if(stInfo.at(iv).second == 1) {
            f1->SetParameter(iv*npars+1,t0); 
            f1->SetParLimits(iv*npars+1,t0-3.0,t0+3.0);
        } else f1->FixParameter(iv*npars+1,t0);
        
        f1->SetParameter(iv*npars+2,10); //f1->SetParameter(iv*npars+3,0.5);
        f1->SetParLimits(iv*npars+2,0.001,1.E6); //f1->SetParLimits(iv*npars+3,0.5,3);
    }    
    
    string iFit;
    //cout  << "Would you like to fit? (y/n)" << endl;
    //cin >> iFit;
    TF1 *fS[nStates];
    iFit = "y";
    if (iFit=="y"){
        hIn->Fit(f1,"NQ","",30,250);
        for (int iv=0;iv<nStates;iv++){
        fS[iv] = new TF1(Form("F%d",iv),calcResponse,0,250,npars);
        //fS[iv] = new TF1("FF",calcResponse,0,250,3);
        fS[iv]->SetNpx(1000);
        if(stInfo.at(iv).second == 0){ 
            fS[iv]->SetLineColor(kBlue);
            fS[iv]->SetLineStyle(9);}
        for (int ip=0;ip<npars;ip++) fS[iv]->SetParameter(ip,f1->GetParameter(iv*npars+ip+1));
        
        cout << "State: " << stInfo.at(iv).first ;
        for (int ip=0;ip<npars;ip++) cout << "\t" << f1->GetParameter(iv*npars+ip+1);
        cout << endl;
        }
    }
    hIn->GetXaxis()->SetRangeUser(35,250);
    hIn->GetYaxis()->SetRangeUser(0,2100);
    hIn->Draw();
    f1->Draw("same");
    for(int i=0; i<nStates; i++) fS[i]->Draw("same");

    return;
}