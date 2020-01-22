#include "tofFuncClass.hpp"

#include "TH1F.h"
#include "TF1.h"

void quickFit(TH1F *hIn,Int_t nPeaks=1, Double_t offMax = 1.0, Bool_t kFix=false){
    Int_t npars=2;
    tofFuncClass tf;
    hIn->GetListOfFunctions()->Clear();
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    TF1 *f1 = new TF1("f1",tf,0,200,npars*nPeaks+2);
    f1->FixParameter(0,nPeaks);
    f1->SetNpx(1000);
    f1->SetLineWidth(4);
    f1->SetLineStyle(9);
    
    Double_t Amax = hIn->GetBinContent(hIn->GetMaximumBin());
    Double_t t0;
    for(int i=0;i<nPeaks;i++){
        cout << "t0 location of " << i+1 << "th peak" << endl;
        cin >> t0;
        if(!kFix){ f1->SetParameter(i*npars+1,t0); f1->SetParLimits(i*npars+1.5,t0-1,t0+1.5);} 
        else f1->FixParameter(i*npars+1,t0);
        
        f1->SetParLimits(i*npars+2,0,Amax);
    }
    f1->SetParLimits(nPeaks*npars+1,0,offMax);

    hIn->Fit(f1,"NL","",25,80);
    cout << "X^2/Ndf:" << f1->GetChisquare()/f1->GetNDF() << endl;
    TF1 *fS[nPeaks];
    
    for (int iv=0;iv<nPeaks;iv++){
        fS[iv] = new TF1(Form("F%d",iv),calcResponse,0,250,npars);
        //fS[iv] = new TF1("FF",calcResponse,0,250,3);
        fS[iv]->SetNpx(1000);
        fS[iv]->SetLineColor(kBlue);
        for (int ip=0;ip<npars;ip++) fS[iv]->SetParameter(ip,f1->GetParameter(iv*npars+ip+1));
    }
    hIn->GetXaxis()->SetRangeUser(25,200);
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