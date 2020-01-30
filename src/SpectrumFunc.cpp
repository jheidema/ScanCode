#include "SpectrumFunc.hpp"

#ifdef SpectrumFunc_hpp

#include <iostream>
#include <fstream>
#include <vector>

#include "tofFuncClass.hpp"
#include "dummyFuncs.hpp"

//#include "TF1.h"

using namespace std;

vector< pair<double, double>  > stInfo;

void LoadInput(const char *fileName, bool kPrint){
    stInfo.clear();
    ifstream fin;
    fin.open(fileName);
    double t0, An, gamE;
    
    while(true){
        fin >> t0 >> An >> gamE;
        if(fin.eof()) break;
        double effScale;
        if (gamE>50.) effScale = 100./AbsHPGe_Eff(gamE)*1.4;
        else effScale=1.0;
        stInfo.push_back(std::make_pair(t0, An*effScale));
        if(kPrint) cout << t0 << "\t"<< An << "\t" << gamE << "\t" << An*effScale << endl;
    }
    
    fin.close();
    return;
}

TF1* SpectrumFunc(int nPeaks, double *x, double *y, double offset, double gamma){

    double scale;

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

TF1* SpectrumFunc(const char *filename, double offset, bool kFit){

    LoadInput(filename, true);
    
    int nPeaks = stInfo.size();
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

#endif