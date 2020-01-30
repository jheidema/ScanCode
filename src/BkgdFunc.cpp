#include "BkgdFunc.hpp"
#ifdef BkgdFunc_hpp

#include "dummyFuncs.hpp"

#include "TH1F.h"
#include "TF1.h"


TF1* BkgdFunc(TH1F * hIn, bool kHyp){
    
    TF1 *pbk;
    TF1 *gbk = new TF1("gbk",gammaBkgd,3,800,7);
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    
    int nPgbk = 0;
    int nPpbk = 0;
    int nPars = 0;
    
    if(kHyp){
    
        pbk = new TF1("pbk",pieceBkgd,-100,800,4);
        /// Fitting the hyperbolic bkgd
        pbk->SetParLimits(0,325,425);
        hIn->Fit(pbk,"QN","",200,600);
        nPpbk = pbk->GetNpar();
    }
    
     nPgbk = gbk->GetNpar();
     nPars = nPgbk + nPpbk;

    TF1 *fbkgd = new TF1("fbkgd",FullBkgd,-100,800,nPars);
    fbkgd->SetParLimits(0,3,4);
    fbkgd->SetParLimits(1,100,1E5);
    fbkgd->SetParLimits(2,0,5);
    fbkgd->SetParLimits(3,100,1E6);
    fbkgd->SetParLimits(4,0,5);
    fbkgd->SetParLimits(5,-1000,1000);
    fbkgd->SetParLimits(6,0,5);

    for (int ig=0;ig<nPars;ig++){
        //if(ig<nPgbk) fbkgd->SetParameter(ig,gbk->GetParameter(ig));
        if(ig>=nPgbk&&ig<nPars) fbkgd->FixParameter(ig,pbk->GetParameter(ig-nPgbk));
    }

    hIn->Fit(fbkgd,"QN","",3,30);
    hIn->Fit(fbkgd,"QN","",3,700);
    
    //hIn->Draw("hist");
    //gbk->Draw("same");

    return fbkgd;
}

#endif