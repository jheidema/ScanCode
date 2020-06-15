#ifndef FitFuncClass_hpp
#define FitFuncClass_hpp

#include <iostream>
#include <cmath>
#include <vector>

#include "calcResponse.hpp"
#include "dummyFuncs.hpp"

#include "TMath.h"

using namespace std;

class FitFuncClass {
    public:
        int nFuncs=0;
        static const int npars = 2;
        int nBP = 0;
        
        double pArray[npars];
        double *bArray;
        double gArray[3];
        double lArray[3];
        vector<double> g_means;
        vector<double> g_sigs;
        vector<double> g_amps;
        int nGaus=0;
    
        vector<double> l_mpvs;
        vector<double> l_sigmas;
        vector<double> l_amps;
        int nLand=0;
    
    FitFuncClass();
    FitFuncClass(int nP, double *initP);
    ~FitFuncClass();
    void SetBGParams(int nP, double *bkP);
    void SetNFuncs(int nfIn){nFuncs=nfIn;};
    
    void AddGaus(double mean, double sigma, double amp){
            g_means.push_back(mean); 
            g_sigs.push_back(sigma); 
            g_amps.push_back(amp); 
            nGaus++; };
    int GetNGaus(){return nGaus;};

    void AddLandau(double mpv, double sigma, double amp){
            l_mpvs.push_back(mpv);
            l_sigmas.push_back(sigma);
            l_amps.push_back(amp);
            nLand++;
    };
    int GetNLand(){return nLand;};

    int  GetNFuncs() {return nFuncs;};
    int  GetNBP() {return nBP;};
    
    double operator() (double *x, double *p){
        double res = 0.;
        if(nBP>0) res += FullBkgd(x,bArray);
        if(nGaus>0){
            for (int ig=0; ig<nGaus; ig++){ 
                gArray[0] = g_means.at(ig); gArray[1] = g_sigs.at(ig); gArray[2] = g_amps.at(ig);
                res += gauss(x,gArray); //amps.at(ig)*exp(-0.5*pow((x[0]-means.at(ig))/sigs.at(ig),2));//*1./(sigs.at(ig)*sqrt(2*3.142));
            }
        }
        if(nLand>0){
            for (int il=0; il<nLand; il++){ 
                res += l_amps.at(il)*TMath::Landau(x[0],l_mpvs.at(il),l_sigmas.at(il),false);
            }
        }

        for (int i=0; i<nFuncs; i++){
            for (int in=0; in<npars; in++) pArray[in] = p[i*npars+in];
            res += calcResponse(x,pArray);
        } 
        
        return res;   
    };
};

#endif