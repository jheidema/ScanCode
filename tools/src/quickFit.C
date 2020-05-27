#include "tofFuncClass.cpp"

#include "TH1F.h"
#include "TF1.h"

void quickFit(TH1F *hIn,Int_t nPeaks=1, Double_t offMax = 1.0, Bool_t kFix=false){

    tofFuncClass tf;
    Int_t npars=tf.npars;
    //cout << npars << endl;
    hIn->GetListOfFunctions()->Clear();
    hIn->GetXaxis()->SetRangeUser(25,150);
    hIn->GetYaxis()->UnZoom();
    
    Double_t maxT=25;
    Double_t minT=150;
    Double_t Amax = hIn->GetBinContent(hIn->GetMaximumBin());
    Double_t t0;
    for(int i=0;i<nPeaks;i++){
        cout << "t0 location of " << i+1 << "th peak" << endl;
        cin >> t0;
        tf.InsertFunction(t0,hIn->GetBinContent(hIn->FindBin(t0)));
        if (t0<minT) minT=t0;
        if (t0>maxT) maxT=t0;
    }
    
    if(nPeaks != tf.GetNFuncs()){
        cout << "Function Incomplete!!\nExiting program... (Inserted: " 
        << nPeaks << ", Returned: " << tf.GetNFuncs() << std::endl; 
        return;
    }
    tf.SortParams();
    tf.SetFitBool(true);

    TF1 *f1 = new TF1("f1",tf,0,200,npars*nPeaks+1);
    //f1->FixParameter(0,nPeaks);
    f1->SetNpx(1000);
    f1->SetLineWidth(4);
    f1->SetLineStyle(9);
    vector <double> fParams = tf.GetParams();
    
    for(int i=0; i<nPeaks; i++){
        if(!kFix){
            f1->SetParameter(i*npars,fParams.at(i*npars)); 
            f1->SetParLimits(i*npars,fParams.at(i*npars)-1,fParams.at(i*npars)+1);
        }else 
            f1->FixParameter(i*npars,fParams.at(i*npars));
        f1->SetParLimits(i*npars+1,0,Amax);
    }
    f1->SetParLimits(nPeaks*npars,0.25,offMax);
    
    hIn->Fit(f1,"NL","",minT-3,maxT+20);
    cout << "X^2/Ndf:" << f1->GetChisquare()/f1->GetNDF() << endl;
    
    TF1 *fS[nPeaks];
    for (int iv=0;iv<nPeaks;iv++){
        fS[iv] = new TF1(Form("F%d",iv),calcResponse,0,250,npars);
        //fS[iv] = new TF1("FF",calcResponse,0,250,3);
        fS[iv]->SetNpx(1000);
        fS[iv]->SetLineColor(kBlue);
        for (int ip=0;ip<npars;ip++) fS[iv]->SetParameter(ip,f1->GetParameter(iv*npars+ip));
    }
    hIn->GetXaxis()->SetRangeUser(25,200);
        
    hIn->Draw();
    f1->Draw("same");
    for(int i=0; i<nPeaks; i++){
        fS[i]->Draw("same");
        for (int ip=0;ip<npars;ip++) cout << f1->GetParameter(i*npars+ip) << "\t";
        cout << endl;
    }
    return;
}