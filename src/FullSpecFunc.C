#ifndef dummyFunc_h
#include "dummyFuncs.h"
#endif
#include "BkgdFunc.C"
#include "HypBkgdFunc.C"
#include "PieceBkgdFunc.C"
#include "SpectrumFunc.hpp"
#include "fullFuncClass.hpp"

#include "TH1F.h"
#include "TF1.h"

///Should add function here to update function with new amplitudes from file
const int nrp = 2;

TF1* GenerateSpecFunc(TH1F * hIn, Bool_t kDraw=true, Bool_t kHyp=false){
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    hIn->GetListOfFunctions()->Clear();
    //Double_t bArray[nBP];
    TF1 *fB = BkgdFunc(hIn,kHyp);
    //for(int ip=0;ip<nBP;ip++) bArray[ip] = fB->GetParameter(ip);
    Double_t *bArray = fB->GetParameters();
    //bArray[8] = bArray[8];
    Int_t nBP = fB->GetNpar();
    
    FullFuncClass tf(nBP,bArray);
    //tf.GetBkgdParams();

    TF1 *fA = SpectrumFunc("FitInfo/In134peaks_all.txt",0.0,false);
    int nFuncs = fA->GetParameter(0);

    for (int iF=0; iF<nFuncs; iF++){
        double t = fA->GetParameter(iF*nrp+1);
        double a = fA->GetParameter(iF*nrp+2);
        tf.InsertFunction(t,a);
    }    

    cout << "Fully Loaded" << endl;
    //tf.GetBkgdParams();
    cout<< tf.GetNFuncs() << endl;
    for (int iF=0; iF<nFuncs; iF++){
        tf.GetFuncParams(iF);
    }

    TF1 *fOut = new TF1("fOut",tf,0,800,0);
    fOut->SetNpx(2000);
    if(kDraw) {hIn->Draw("hist");fOut->Draw("same");}

    return fOut;
}