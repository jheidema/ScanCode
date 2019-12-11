#ifndef dummyFunc_h
#include "dummyFuncs.h"
#endif

#include "TH1F.h"
#include "TF1.h"


TF1* GeneratePieceBkgdFunc(TH1F * hIn, Bool_t kHyp=false){

    TF1 *pbk = new TF1("pbk",pieceBkgd,-100,800,4);
    TF1 *gbk = new TF1("gbk",gammaBkgd,3,800,6);
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();

    if(kHyp){
        /// Fitting the hyperbolic bkgd
        pbk->SetParLimits(0,325,425);
        hIn->Fit(pbk,"N","",250,600);
    }
    
//    hIn->Draw();
//    pbk->Draw("same");
//return;
    /// Create bkgd histogram
    Int_t nbins = hIn->GetNbinsX();
    Double_t xmin = hIn->GetXaxis()->GetXmin();
    Double_t xmax = hIn->GetXaxis()->GetXmax();

    if(kHyp){
        Double_t pb_int = pbk->Integral(xmin,xmax);

        TH1F *pb = new TH1F("pb","pb",nbins,xmin,xmax);
        pb->FillRandom("pbk",pb_int*2);

        /// Scaling bkgd histogram
        Int_t pbk_max_bin = pb->GetBinCenter(pb->FindBin(pbk->GetParameter(0)));
        pb_int = pb->Integral(pbk_max_bin-50,pbk_max_bin+50);
        Double_t hIn_int = hIn->Integral(pbk_max_bin-50,pbk_max_bin+50);
        Double_t scale = hIn_int/pb_int;
        pb->Scale(scale);
        std::cout << scale << std::endl;

        /// Subtract bkgd histogram
        //hIn->Sumw2();
        //pb->Sumw2();
        hIn->Add(pb,-0.85);
        //hIn->Draw();
        //pb->Draw("same");
        //pbk->Draw("same");
        //return;


        //for (int ib=1; ib<=nbins; ib++){ if(hIn->GetBinContent(ib)<0) hIn->SetBinContent(ib,0);}
        pb->Delete(); //delete bkgd histogram
    }    

//return;

    gbk->SetParLimits(0,3,4);
    gbk->SetParLimits(1,100,1E5);
    gbk->SetParLimits(2,0,5);
    gbk->SetParLimits(3,100,1E6);
    gbk->SetParLimits(4,0,5);
    gbk->SetParLimits(5,-1000,1000);

    hIn->Fit(gbk,"N","",3,32);
    hIn->Fit(gbk,"N","",250,400);
    
    //Double_t offset = gbk->GetParameter(5)-25.0*gbk->GetParError(5);
    //cout << offset << endl;
    //gbk->FixParameter(5,offset);
    gbk->FixParameter(5,gbk->GetParameter(5));

    hIn->Fit(gbk,"N","",3,35);
//    hIn->Draw("hist");
//    gbk->Draw("same");
//return;

    Int_t nPars = gbk->GetNpar() + pbk->GetNpar();

    TF1 *fbkgd = new TF1("fbkgd",FullBkgd,-100,800,nPars);
    for (int ig=0;ig<nPars;ig++){
        if(ig<6) fbkgd->SetParameter(ig,gbk->GetParameter(ig));
        if(ig>=6&&ig<11) fbkgd->SetParameter(ig,pbk->GetParameter(ig-6));
    }
    fbkgd->SetParameter(5,0);
    return fbkgd;
}