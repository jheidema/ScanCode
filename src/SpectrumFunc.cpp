#include "SpectrumFunc.hpp"

#ifdef SpectrumFunc_hpp

#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>

#include "tofFuncClass.hpp"
#include "dummyFuncs.hpp"

#include "VandleEff.hpp"
#include "TGraph.h"

using namespace std;

vector< pair<double, double> > stInfo;
map<double, double> feedR;
const TGraph *gEff = Efficiencies::VandleSourceEff();

double CalcEn(double tof){ //return MeV
    double ee = 0.5*mn/pow(c,2)*pow(100/tof,2);
    return ee;
}
double CalcTOF(double ee){ //return ns
    double tof = sqrt(0.5*mn/ee)*(100.0/c);
    return tof;
}

void LoadInput(const char *fileName, double fGS, bool kPrint){
    stInfo.clear();
    ifstream fin;
    fin.open(fileName);
    double t0, An, gamE, Ex, tgs, neffR;
    
    while(true){
        fin >> t0 >> An >> gamE >> Ex;
        if(fin.eof()) break;
        double effScale;
        double en = CalcEn(t0);
        if (gamE>50.) effScale = 100./AbsHPGe_Eff(gamE)*1.0;
        else effScale=0.25;
        stInfo.push_back(std::make_pair(t0, An*effScale));
        if(gamE==1561&&fGS>0.0){
            double gEn = (Ex-Sn)/1000.;
            tgs = CalcTOF(gEn);
            neffR = gEff->Eval(gEn)/gEff->Eval(en);
            stInfo.push_back(std::make_pair(tgs,An*effScale*feedR.at(1561)*neffR*fGS));
            printf("GS Transition: %.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",t0,en,tgs,gEn,neffR);
        }
        if(kPrint) cout << t0 << "\t"<< An << "\t" << gamE << "\t" << An*effScale << endl;
    }
    fin.close();
    return;
}



TF1* SpectrumFunc(const char *filename, double offset, double fGS){

    feedR.insert(std::make_pair(1561.,5.));
    feedR.insert(std::make_pair(854.,25.));
    LoadInput(filename, fGS);
    
    int nPeaks = stInfo.size();
    tofFuncClass tf;

    TF1 *ff = new TF1("ff",tf,0,200,nPeaks*npars+2);
    ff->FixParameter(0,nPeaks);
    ff->SetNpx(2000);

    for(int i=0;i<nPeaks;i++){
        ff->FixParameter(i*npars+1,stInfo.at(i).first);
        /*if(kFit) {ff->SetParameter(i*npars+2,stInfo.at(i).second); ff->SetParLimits(i*npars+2,0,1E9);}
        else*/ ff->FixParameter(i*npars+2,stInfo.at(i).second);
    }

    if (offset > 0) ff->FixParameter(nPeaks*npars+1,offset);
    else ff->FixParameter(nPeaks*npars+1,0);

    return ff;
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



#endif