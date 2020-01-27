#include "dummyFuncs.h"

#include "TF1.h"
#include "TH2F.h"
#include "TGraphErrors.h"



using namespace std;

void WalkParams(TH2F *h){

    Double_t xmax = h->GetXaxis()->GetXmax();
    Double_t ymax = h->GetYaxis()->GetXmax();
    Double_t xmin = h->GetXaxis()->GetXmin();
    Double_t ymin = h->GetYaxis()->GetXmin();
    Int_t   binsX = h->GetXaxis()->GetNbins();
    Int_t   binsY = h->GetYaxis()->GetNbins();

    TGraphErrors *gCorr = new TGraphErrors();
    gCorr->SetName("gCorr");

    if (xmax > 1000 && ymax < 1000){
        for (int ix = 0; ix<binsX; ix++){
            Int_t maxBin = h->ProjectionY("",ix,ix)->GetMaximumBin();
            Double_t max = h->ProjectionY()->GetBinCenter(maxBin);
            Double_t xloc = h->ProjectionX()->GetBinCenter(ix);
            gCorr->SetPoint(ix,xloc,max);
            gCorr->SetPointError(ix,0,0.05);
        }
    }
    else if (ymax > 1000 && xmax < 1000){
        for (int iy = 0; iy<binsY; iy++){
            Int_t maxBin = h->ProjectionX("",iy,iy)->GetMaximumBin();
            Double_t max = h->ProjectionX()->GetBinCenter(maxBin);
            Double_t xloc = h->ProjectionY()->GetBinCenter(iy);
            gCorr->SetPoint(iy,xloc,max);
            gCorr->SetPointError(iy,0,0.05);
        }
    }
    else{cout << "Histogram can't be processed correctly. " << endl; return;}
    gCorr->Draw("AP");

    TF1 *fCorr = new TF1("fCorr",tofWcorr,500,25000,6);
    fCorr->SetParLimits(0,6000,8000);
    fCorr->SetParLimits(1,2.4,2.8);
    fCorr->SetParLimits(2,0.1,1);
    fCorr->SetParLimits(3,-0.01,0);
        
    gCorr->Fit(fCorr,"R");
 
    gCorr->Draw("AP");

    cout << "Parameters\n{" ;
    for (int i=0;i<6;i++){
        cout << fCorr->GetParameter(i); if (i<5) cout << ",";  
    }
    cout << "}" << endl;
return;
}