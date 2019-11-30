#include "tofFuncClass.hpp"

#include "TH1F.h"
#include "TF1.h"

void quickFit(TH1F *hIn,Int_t nPeaks=1,Bool_t kFix=false){
    
    tofFuncClass tf;
    hIn->GetListOfFunctions()->Clear();
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    TF1 *f1 = new TF1("f1",tf,0,200,2*nPeaks+2);
    f1->FixParameter(0,nPeaks);
    f1->SetNpx(1000);
    f1->SetLineWidth(4);
    f1->SetLineStyle(9);
    
    Double_t t0;
    for(int i=0;i<nPeaks;i++){
        cout << "t0 location of " << i+1 << "th peak" << endl;
        cin >> t0;
        if(!kFix) f1->SetParameter(i*2+1,t0);
        else f1->FixParameter(i*2+1,t0);
    }
    for(int i=0;i<nPeaks;i++) {
        if(!kFix) f1->SetParLimits(i*2+1,f1->GetParameter(i*2+1)-1,f1->GetParameter(i*2+1)+1);
        f1->SetParLimits(i*2+2,0,hIn->GetBinContent(hIn->GetMaximumBin()));
    }
    f1->SetParLimits(nPeaks*2+1,0,1);

    hIn->Fit(f1,"NL","",35,125);
    TF1 *fS[nPeaks];
    Int_t npars=2;
    for (int iv=0;iv<nPeaks;iv++){
        fS[iv] = new TF1(Form("F%d",iv),calcResponse,0,250,npars);
        //fS[iv] = new TF1("FF",calcResponse,0,250,3);
        fS[iv]->SetNpx(1000);
        fS[iv]->SetLineColor(kBlue);
        for (int ip=0;ip<npars;ip++) fS[iv]->SetParameter(ip,f1->GetParameter(iv*npars+ip+1));
    }
    hIn->GetXaxis()->SetRangeUser(35,250);
    //hIn->GetYaxis()->SetRangeUser(0,2100);
    
    hIn->Draw();
    f1->Draw("same");
    for(int i=0; i<nPeaks; i++){
        fS[i]->Draw("same");
        for (int ip=0;ip<npars;ip++) cout << f1->GetParameter(i*npars+ip+1) << "\t";
        cout << endl;
    }
    return;
}