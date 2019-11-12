#include "tofFuncClass.hpp"

void testFitSpec(TH1F *hIn,Int_t nPeaks=1){
    
    TSpectrum *s1 = new TSpectrum(nPeaks);
    s1->Search(hIn,1,"",0.05);

    double *x = s1->GetPositionX();
    double *y = s1->GetPositionY();
    int np = s1->GetNPeaks();

    tofFuncClass tf;
    
    TF1 *f1 = new TF1("f1",tf,0,200,3*np+2);
    f1->FixParameter(0,np);
    f1->SetNpx(1000);

    for (int ip=0; ip<np; ip++){
        f1->SetParameter(ip*3+1,x[ip]);f1->SetParameter(ip*3+2,y[ip]);f1->SetParameter(ip*3+3,0.2);
        f1->SetParLimits(ip*3+1,x[ip]-1.5,x[ip]+1.5);f1->SetParLimits(ip*3+3,0,3);
    }

    f1->Draw("same");

    string iFit;
    cout  << "Would you like to fit? (y/n)" << endl;
    getline(cin,iFit);
    
    if (iFit=="y") hIn->Fit(f1,"","",30,150);

    return;
}