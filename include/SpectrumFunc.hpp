#include "tofFuncClass.hpp"

#ifndef dummyFunc_h
#include "dummyFuncs.h"
#endif

#include "TF1.h"
#include "TH1F.h"
#include <iostream>
#include <fstream>

vector< pair<Double_t, Double_t>  > stInfo;
const Double_t mn = 939.57; //MeV/c^2
const Double_t c = 2.998E8;
const int npars = 2;

void LoadInput(const char *fileName, Bool_t kPrint=false){
    stInfo.clear();
    ifstream fin;
    fin.open(fileName);
    Double_t t0, An, gamE;
    
    while(true){
        fin >> t0 >> An >> gamE;
        if(fin.eof()) break;
        Double_t effScale;
        if (gamE>50.) effScale = 100./AbsHPGe_Eff(gamE)*1.4;
        else effScale=1.0;
        stInfo.push_back(std::make_pair(t0, An*effScale));
        if(kPrint) cout << t0 << "\t"<< An << "\t" << gamE << "\t" << An*effScale << endl;
    }
    
    fin.close();
    return;
}

TF1* SpectrumFunc(Int_t nPeaks, Double_t *x, Double_t *y, Double_t offset=0.0, Double_t gamma=0.0){

    Double_t scale;

    if(gamma>10){
        scale = 100./AbsHPGe_Eff(gamma);
    } else scale = 1.0;
    cout << scale << endl;

    tofFuncClass tf;

    TF1 *ff = new TF1("ff",tf,0,200,nPeaks*npars+2);
    ff->FixParameter(0,nPeaks);
    ff->SetNpx(1000);
    for(int i=0;i<nPeaks;i++){
        ff->FixParameter(i*npars+1,x[i]);
        ff->FixParameter(i*npars+2,y[i]*scale);
    }
    if (offset > 0) ff->FixParameter(nPeaks*npars+1,offset*scale);
    else ff->FixParameter(nPeaks*npars+1,0);

    return ff;
}

TF1* SpectrumFunc(const char *filename, Double_t offset=0.0, Bool_t kFit=false){

    LoadInput(filename, false);
    
    Int_t nPeaks = stInfo.size();
    tofFuncClass tf;

    TF1 *ff = new TF1("ff",tf,0,200,nPeaks*npars+2);
    ff->FixParameter(0,nPeaks);
    ff->SetNpx(2000);

    for(int i=0;i<nPeaks;i++){
        ff->FixParameter(i*npars+1,stInfo.at(i).first);
        if(kFit) {ff->SetParameter(i*npars+2,stInfo.at(i).second); ff->SetParLimits(i*npars+2,0,1E9);}
        else ff->FixParameter(i*npars+2,stInfo.at(i).second);
    }

    if (offset > 0) ff->FixParameter(nPeaks*npars+1,offset);
    else ff->FixParameter(nPeaks*npars+1,0);

    return ff;
}

