#include "SpectrumClass.hpp"

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


const TGraph *vEff = Efficiencies::VandleSourceEff();

const double mn = 939.57; //MeV/c^2
const double c = 29.98; //cm/ns
const int npars = 2;
const double Sn = 3631; //134Sn neutron separation energy in keV

SpectrumFunc::SpectrumFunc(){
    feedR.insert(std::make_pair(1561.,5.));
    feedR.insert(std::make_pair(854.,25.));
};

SpectrumFunc::~SpectrumFunc(){};

double SpectrumFunc::CalcEn(double tof){
    double ee = 0.5*mn/pow(c,2)*pow(100/tof,2);
    return ee;
}
double SpectrumFunc::CalcTOF(double ee){
    double tof = sqrt(0.5*mn/ee)*(100.0/c);
    return tof;
}

void SpectrumFunc::PrintExBR(){
    for (std::map<double,std::pair<double,double>>::iterator it = exBR.begin(); it!=exBR.end(); it++){
        cout << "134Sn Ex: " << it->first << "\tGam: " << it->second.first << "\tIx: " << it->second.second << endl; 
    }
}

/* void SpectrumFunc::SortGS(){
    if(gsInfo.size()<2) return;
    //std::vector< std::pair<double, double> >
    double pvals[2];
    vector<double> copy_vec;
    copy_vec.push_back(params.at(0));
    copy_vec.push_back(params.at(1));
    std::vector<double>::iterator it;

    for(int ip=2; ip<nFuncs*npars; ip=ip+2){
        pvals[0]=params.at(ip);
        pvals[1]=params.at(ip+1);
        for(it = copy_vec.begin(); it<copy_vec.end(); it++){
            if(params.at(ip)<*it)  break;
        }
        copy_vec.insert(it,pvals,pvals+2);   
    }
    params.clear();
    params = copy_vec;
} */



void SpectrumFunc::LoadInput(const char *fileName, bool kGS, bool kPrint){
    stInfo.clear();
    gsInfo.clear();
    ifstream fin;
    fin.open(fileName);
    double t0, An, gamE, Ex, tgs, neffR, gsBR;
    
    while(true){
        fin >> t0 >> An >> gamE >> Ex >> gsBR;
        double gEff;
        double en = CalcEn(t0);

        if (gamE>50.) gEff = 100./AbsHPGe_Eff(gamE);
        else {cout << "Gamma E too low for eff. correction\n"; gEff=0.25;}
        
        stInfo.push_back(std::make_pair(t0, An*gEff));
        exBR.insert(std::make_pair(Ex,std::make_pair(gamE,An*100/vEff->Eval(en)*gEff)));
        double gEn = (Ex-Sn)/1000.;
        tgs = CalcTOF(gEn);
        neffR = vEff->Eval(gEn)/vEff->Eval(en);
        
        if(gsBR>0){
            gsInfo.push_back(std::make_pair(tgs,An*gEff*neffR*gsBR));
        if(kGS){
            stInfo.push_back(std::make_pair(tgs,An*gEff*neffR*gsBR));
            if(kVerbose) printf("GS Transition: %.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",t0,en,tgs,gEn,neffR,An*gEff*neffR*gsBR);
        }
        }
        if(kPrint) cout << t0 << "\t"<< An << "\t" << gamE << "\t" << An*gEff << endl;
        if(fin.eof()) break;
    }
    fin.close();
    return;
}



TF1* SpectrumFunc::CreateFunc(const char *filename, double offset, bool kGS){

    LoadInput(filename, kGS);
    
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


TF1* SpectrumFunc::CreateFunc(int nPeaks, double *x, double *y, double offset, double gamma){

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