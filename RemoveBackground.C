#include "dummyFuncs.h"

void RemoveBackground(TH1F *hIn){
    TF1 *hbk = new TF1("hbk",hypBkgd,-50,800,5);
    TF1 *gbk = new TF1("gbk",gammaBkgd,3,800,6);
    
    /// Fitting the hyperbolic bkgd
    hbk->SetParLimits(0,-15,-5);
    hbk->SetParLimits(1,0.2,3);
    hbk->SetParLimits(2,0.2,3);
    hbk->SetParLimits(3,325,425);
    hbk->SetParLimits(4,13500,14500);
    hIn->Fit(hbk,"","",200,600);

    /// Create bkgd histogram
    Int_t nbins = hIn->GetNbinsX();
    Double_t xmin = hIn->GetXaxis()->GetXmin();
    Double_t xmax = hIn->GetXaxis()->GetXmax();

    Double_t hb_int = hbk->Integral(xmin,xmax);

    TH1F *hb = new TH1F("hb","hb",nbins,xmin,xmax);
    hb->FillRandom("hbk",hb_int/2);

    /// Scaling bkgd histogram
    Int_t hbk_max_bin = hb->GetBinCenter(hb->FindBin(hbk->GetParameter(3)));
    hb_int = hb->Integral(hbk_max_bin-50,hbk_max_bin+50);
    Double_t hIn_int = hIn->Integral(hbk_max_bin-50,hbk_max_bin+50);
    Double_t scale = hIn_int/hb_int;
    hb->Scale(scale);
    std::cout << scale << std::endl;
    
    /// Subtract bkgd histogram
    hIn->Add(hb,-1.0);
    //hb->Draw("same");
return;
    for (int ib=1; ib<=nbins; ib++){ if(hIn->GetBinContent(ib)<0) hIn->SetBinContent(ib,0);}

    hb->Delete(); //delete bkgd histogram

    gbk->SetParLimits(0,3,4);
    gbk->SetParLimits(1,100,1E5);
    gbk->SetParLimits(2,0,5);
    gbk->SetParLimits(3,100,1E6);
    gbk->SetParLimits(4,0,5);
    gbk->SetParLimits(5,0,1000);

    hIn->Fit(gbk,"N","",3,35);
    hIn->Fit(gbk,"N","",300,600);
    
    gbk->FixParameter(5,gbk->GetParameter(5));

    hIn->Fit(gbk,"N","",3,35);
    

    double xx, yy;
    double gy;
    UInt_t YY;
    for (int ib=1; ib<=nbins; ib++){
        xx=hIn->GetBinCenter(ib); yy=hIn->GetBinContent(ib);
        gy=gbk->Eval(xx);
        if(xx<0) YY = 0;
        else if(yy<gy) YY = 0;
        else YY = yy - gy;
        hIn->SetBinContent(ib,YY);
    }

    return;
}