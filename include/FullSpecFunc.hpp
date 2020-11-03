#ifndef FullSpecFunc_hpp
#define FullSpecFunc_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "FullFuncClass.hpp"
#include "FileReader.hpp"
#include "SpectrumClass.hpp"
#include "FitFuncClass.hpp"

#include "TH1F.h"
#include "TF1.h"


class FullSpecFunc {
    public:
    FullSpecFunc();
    FullSpecFunc(bool kVerbose_);
    ~FullSpecFunc();

    void SetInfoFile(const char *filename_){filename = filename_;}; 
    bool OpenFile();
    void GenerateSpecFunc();
    void GenerateSpecFunc(TH1D *hIn, bool kPrint=false);
    TF1* GenerateFitFunc(TH1D* hIn, bool kGNFit=false);

    void SetGSCalc(bool kIN) {kGS = kIN;};
    void SetTailFit(bool kIN) {kFitTail=kIN;};
    void SetGSFloat(bool kIN) {kFloatGS=kIN;};

    void AddFuncs(FitFuncClass &gff, const char *filename_);
    void SetFuncFileName(const char *filename_){funcfilename=filename_;};
    FullFuncClass GetFuncObj(){return tf;};
    SpectrumFunc GetSpecFunc(){return sf;};
    
    TF1* GetFunc(){return fOut;};
    
    int GetNgauss(){return gf.GetNGaus();};
    void GetGaussPars(double *prs, int igs){
        if(igs>ngs){prs[0]=0; prs[1]=0; prs[2]=0; return;}
        prs[0] =  gf.g_means.at(igs);
        prs[1] =  gf.g_sigs.at(igs);
        prs[2] =  gf.g_amps.at(igs);
        };
    double *GetGaussPars(int igs){
        if(igs>=ngs){pars[0]=0; pars[1]=0; pars[2]=0; return pars;}
        pars[0] =  gf.g_means.at(igs);
        pars[1] =  gf.g_sigs.at(igs);
        pars[2] =  gf.g_amps.at(igs);
        return pars;
        };
    
    int GetNlandau(){return gf.GetNLand();};
    void GetLandauPars(double *prs, int ild){
        if(ild>nld){prs[0]=0; prs[1]=0; prs[2]=0; return;}
        prs[0] =  gf.l_mpvs.at(ild);
        prs[1] =  gf.l_sigmas.at(ild);
        prs[2] =  gf.l_amps.at(ild);
        };
    double *GetLandauPars(int ild){
        if(ild>=nld){pars[0]=0; pars[1]=0; pars[2]=0; return pars;}
        pars[0] =  gf.l_mpvs.at(ild);
        pars[1] =  gf.l_sigmas.at(ild);
        pars[2] =  gf.l_amps.at(ild);
        return pars;
        };

    int GetNFuncInput(){return gf.GetNNu();}
    
    std::map<double, std::pair<double, double> > GetSpectrumGNInfo() {return sf.GetGNInfo();};
    int GetNGammaNuStates() {return tf.GetNFuncs();};

    FileReader fr;
    FileReader funcf;
    SpectrumFunc sf;
    FullFuncClass tf;
    FitFuncClass gf;
    
    const int nrp = 2;
    const char *filename;
    const char *funcfilename;
    TF1 *fB;
    TF1 *fOut;
    double *bArray;
    
    int nBP=0;
    int nFuncs=0;
    int ngs=0;
    int nld=0;
    int nnu=0;

    bool kGS = false;
    bool kVerbose = false;
    bool kFitTail = false;
    bool kFloatGS = false;

    private:
        double pars[3]={0};
};

#endif