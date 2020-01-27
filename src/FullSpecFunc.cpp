#include "TH1F.h"
#include "TF1.h"

#include "BkgdFunc.C"
//#include "HypBkgdFunc.C"
//#include "PieceBkgdFunc.C"
#include "SpectrumFunc.hpp"
//#include "fullFuncClass.hpp"

#include "FullSpecFunc.hpp"

#ifdef FullSpecFunc_hpp

using namespace std;

FullSpecFunc::FullSpecFunc(){};

FullSpecFunc::~FullSpecFunc(){};

void FullSpecFunc::GenerateSpecFunc(TH1F * hIn, Bool_t kHyp=false){
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    hIn->GetListOfFunctions()->Clear();
    //Double_t bArray[nBP];
    fB = BkgdFunc(hIn,kHyp);
    //for(int ip=0;ip<nBP;ip++) bArray[ip] = fB->GetParameter(ip);
    bArray = fB->GetParameters();
    //bArray[8] = bArray[8];
    nBP = fB->GetNpar();
    
    FullFuncClass tf(nBP,bArray);
    //tf.GetBkgdParams();

    TF1 *fA = SpectrumFunc(filename,0.0,false);
    nFuncs = fA->GetParameter(0);

    for (int iF=0; iF<nFuncs; iF++){
        double t = fA->GetParameter(iF*nrp+1);
        double a = fA->GetParameter(iF*nrp+2);
        tf.InsertFunction(t,a);
    }    

    //std::cout << "Fully Loaded\n";
    //tf.GetBkgdParams();
    //cout << tf.GetNFuncs() << endl;
    
    /* for (int iF=0; iF<nFuncs; iF++){
        tf.GetFuncParams(iF);
    } */

    fOut = new TF1("fOut",tf,0,800,0);
    fOut->SetNpx(2000);
    //if(kDraw) {hIn->Draw("hist");fOut->Draw("same");}
}

#endif