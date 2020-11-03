#include "tofFuncClass.cpp"

#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TApplication.h"

#include "../include/MakeAxis.h"

void addPeak(TH1F *hIn, Double_t offMax = 1.0, Bool_t kFix=false){
    
    gStyle->SetOptStat(0);

    tofFuncClass tf;
    Int_t npars=tf.npars;
    tf.SetFitBool(true);
    cout << tf.GetFitBool() << endl;

    hIn->GetListOfFunctions()->Clear();
    hIn->GetXaxis()->SetRangeUser(30,100);
    hIn->GetYaxis()->UnZoom();
    hIn->SetLineStyle(0);
    hIn->SetLineColor(kBlack);
    
    TCanvas *c1 = new TCanvas("c1","c1",1000,600);
    hIn->Draw();

    TF1 *f1 = new TF1();
    std::string inString;
    Int_t npeaks=0;

    Double_t maxT=25;
    Double_t minT=150;

    Double_t xlow=30;
    Double_t xhi=100;


    Double_t Amax = hIn->GetBinContent(hIn->GetMaximumBin());
    Double_t t0;
    int rF;

    int fMode=0;
    std::map<int,bool> pLock;
    bool kRun;

  while(true){
      kRun=false;
    c1->Update();
    cout<< "Next Step? [+/add,-/remove,0/done,(U)L/(un)lock,(U)Lall,run,ftoggle]" << endl;
    cin >> inString;

    //c1->WaitPrimitive();

    if(inString=="0" || inString=="done") break;

    if(inString=="run") kRun=true;

    if(inString=="xzoom"){
        cout << "New X axis range: <low> <high>\n";
        cin >> xlow >> xhi;
        hIn->GetXaxis()->SetRangeUser(xlow,xhi);
        //hIn->Draw();
        gPad->Modified();
        gPad->Update();
    }

    if(inString=="ftoggle"){
        fMode=abs(fMode-1);
        if(fMode==0) cout << "Likelihood Fitting" << endl;
        else if(fMode==1) cout << "Minuit Fitting" << endl;
    }
    
    if(inString=="+" || inString=="add"){  
        cout << "t0 location of " << npeaks+1 << "th peak" << endl;
        cin >> t0;
        if(t0>30 && t0<120){
            tf.InsertFunction(t0,hIn->GetBinContent(hIn->FindBin(t0)));
            if (t0<minT) minT=t0;
            if (t0>maxT) maxT=t0;
            kRun=true;
        } else cout << "Peak not allowed" << endl;
    }
    if(inString=="++"){  
        while (true){
            cout << "t0 location of " << npeaks+1 << "th peak [enter 0 to stop]" << endl;
            cin >> t0;
            if(t0>30 && t0<120){
                tf.InsertFunction(t0,hIn->GetBinContent(hIn->FindBin(t0)));
                if (t0<minT) minT=t0;
                if (t0>maxT) maxT=t0;
            }else break;
        }
        kRun=true;
    }

    //tf.SortParams(); //sort parameters only after adding a new peak or before removing a peak
    
    if(inString=="-" || inString=="remove"){
        if(npeaks>0){
            tf.PrintFuncParams();
            cout << "Function number to remove? [1," << npeaks << "]" << endl;
            cin >> rF;
            tf.RemoveFunction(rF);
            kRun=true;
        } else cout << "No Functions to remove." << endl;
    }

    if(inString=="lock" || inString=="L"){
        cout << "Lock which function?" << endl;
        tf.PrintFuncParams();
        cin >> rF;
        pLock[rF]=true;
    }
    if(inString=="Lall"){
        for (int i=1;i<=npeaks;i++) pLock[i]=true;
    }

    if(inString=="unlock" || inString=="UL"){
        cout << "Unlock which function?" << endl;
        for(std::map<int,bool>::iterator it = pLock.begin(); it!=pLock.end(); it++){
            cout << it->first << " " << it->second << endl;
        }
        cin >> rF;
        pLock[rF]=false;
    }
    if(inString=="ULall"){
        for (int i=1;i<=npeaks;i++) pLock[i]=false;
    }

    npeaks = tf.GetNFuncs();
    
   if(npeaks>0 && kRun){
    //tf.SetFitBool(true);
    f1->Delete();
    f1 = new TF1("f1",tf,0,200,npars*npeaks+1);
    
    f1->SetNpx(1000);
    f1->SetLineWidth(4);
    f1->SetLineStyle(9);
    vector <double> fParams = tf.GetParams();
    for(int i=0; i<npeaks; i++){
        double t=fParams.at(i*npars);
        double a=fParams.at(i*npars+1);
        if(pLock[i+1]){
            f1->FixParameter(i*npars,t);
            f1->FixParameter(i*npars+1,a);
        }else{
            f1->SetParameter(i*npars,t); 
            f1->SetParLimits(i*npars,t-1,t+1);
            f1->SetParLimits(i*npars+1,0,a*2.0);
        }
    }
    f1->SetParameter(npeaks*npars,0.25);
    f1->SetParLimits(npeaks*npars,0.1,offMax);
    
    if(fMode==0) hIn->Fit(f1,"QNL","",minT-3,maxT+3);
    else if(fMode==1) hIn->Fit(f1,"QN","",minT-3,maxT+3);

    cout << "X^2/Ndf:" << f1->GetChisquare()/f1->GetNDF() << endl;
    tf.SetParams(f1->GetParameters());
    TF1 *fS[npeaks];
    for (int iv=0;iv<npeaks;iv++){
        fS[iv] = new TF1(Form("F%d",iv),calcResponse,0,250,npars);
        fS[iv]->SetNpx(1000);
        fS[iv]->SetLineColor(kBlue);
        for (int ip=0;ip<npars;ip++) fS[iv]->SetParameter(ip,f1->GetParameter(iv*npars+ip));
    }
    
        
    hIn->GetXaxis()->SetRangeUser(xlow,xhi);
    //hIn->GetYaxis()->SetRangeUser(0,Amax*1.3);
    TGaxis *a1 = MakeAxis(hIn);
    hIn->Draw();
    a1->Draw("same");
    f1->Draw("same");
    for(int i=0; i<npeaks; i++){
        fS[i]->Draw("same");
        for (int ip=0;ip<npars;ip++) cout << f1->GetParameter(i*npars+ip) << "\t";
        cout << endl;
    }
    cout << f1->GetParameter(npeaks*npars) << endl;
   }
  }
    return;
}

void application(int argc, char** argv){
    cout << *argv << endl;
    cout << argv[1] << endl; 
    TFile *fIn = new TFile(argv[1],"READ");
    TH1F *hIn = (TH1F*)fIn->Get(argv[2]);
    
    addPeak(hIn);
    
    return;
}

int main(int argc, char** argv){
    TApplication app("Peak Application", &argc, argv);
    application(app.Argc(), app.Argv());
    app.Run(true);
    return 0;
}