#ifndef dummyFunc_h
#include "dummyFuncs.hpp"
#endif
#include "BackGroundFunc.C"
#include "SpectrumFunc.hpp"
#include "fullFuncClass.hpp"

#include "TH1F.h"
#include "TF1.h"


TF1* GenerateSpecFunc(TH1F * hIn, Bool_t kHyp=false){
    Double_t bArray[11];
    TF1 *fB = GenerateHypBkgdFunc(hIn,kHyp);
    for(int ip=0;ip<11;ip++) bArray[ip] = fB->GetParameter(ip);
    
    FullFuncClass tf(11,bArray);
    
    TF1 *fA = SpectrumFunc("FitInfo/In134peaks_all.txt",0.0,false);
    int nFuncs = fA->GetParameter(0);

    for (int iF=0; iF<nFuncs; iF++){
        double t = fA->GetParameter(iF*2+1);
        double a = fA->GetParameter(iF*2+2);
        tf.InsertFunction(t,a);
    }    

    cout<< tf.GetNFuncs() << endl;
    for (int iF=0; iF<nFuncs; iF++){
        tf.GetFuncParams(iF);
    }

    TF1 *fOut = new TF1("fOut",tf,0,800,0);
    fOut->SetNpx(2000);
    return fOut;
}