#ifndef dummyFunc_h
#include "dummyFuncs.h"
#endif

#include "TH1F.h"
#include "TF1.h"


TF1* BkgdFunc(TH1F * hIn, Bool_t kHyp=false){
    
    TF1 *pbk = new TF1("pbk",pieceBkgd,-100,800,4);
    TF1 *gbk = new TF1("gbk",gammaBkgd,3,800,7);
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();

    if(kHyp){
        /// Fitting the hyperbolic bkgd
        pbk->SetParLimits(0,325,425);
        hIn->Fit(pbk,"N","",200,600);
    }
    
    Int_t nPgbk = gbk->GetNpar();
    Int_t nPpbk = pbk->GetNpar();
    Int_t nPars = nPgbk + nPpbk;

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

    hIn->Fit(fbkgd,"N","",3,20);
    hIn->Fit(fbkgd,"N","",3,700);
    
    //hIn->Draw("hist");
    //gbk->Draw("same");

    return fbkgd;
}