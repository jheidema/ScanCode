#include "nResponse.hpp"
#include <iostream>

#include "TF1.h"
#include "TH1D.h"
#include "TH1F.h"

using namespace std;

TF1* generateResponse(TH1D *hIn, Bool_t kFit=false){

    double t0;
    hIn->GetListOfFunctions()->Clear();
    //double yMax = hIn->GetBinContent(hIn->GetMaximumBin());
    //double upperX = hIn->GetBinCenter(hIn->GetNbinsX());
    TF1 *fTOF = new TF1("fTOF",nResponse,0,1000,11);
    fTOF->SetNpx(1000);
    fTOF->SetParNames("t0","a0","sig0","sig1","k1","d1","k2","d2","k3","d3","AA");
    //fTOF->SetParameter(0,xMax);
    //fTOF->SetParameter(10,yMax);
    TCanvas *c1 = new TCanvas();
    hIn->Draw();
    c1->Update();

    cout << "Peak t0? (enter 0 for exit)" << endl;
    cin >> t0;
        if (t0<=0) return NULL;
    cout << "Fitting Response around " << t0 << endl;

    double a0 = 0.1667 + (-0.00141)*t0 + (5.27e-6)*t0*t0;
    double sig0 = 0.2594 + (0.000699)*t0 + 0.000202*t0*t0;
    double sig1 = -0.139 + 0.0242*t0;
    double k1 = 0.1819 - 0.000118*t0 + (4.215e-6)*t0*t0;
    double d1 = 75.41/(t0+17.70)+1.878;
    double k2 = 0.0162 - (4.797e-5)*t0 + (1.887e-6)*t0*t0;
    double d2 = 8.684 + 0.1331*t0 - (3.685e-4)*t0*t0;
    double k3 = 0.00335 + (2.108e-4)*t0 - (6.485e-7)*t0*t0;
    double d3 = 320.057 - 2.324*t0 + 0.00441*t0*t0;

    double AA = hIn->GetBinContent(hIn->FindBin(t0));
    hIn->GetXaxis()->UnZoom();
    fTOF->SetParameters(t0,a0,sig0,sig1,k1,d1,k2,d2,k3,d3,AA);
    double lowB, upB;
    if(kFit){
        cout << "Fitting Range [lower upper]:";
        cin >> lowB >> upB;
        if(lowB<upB && lowB!=0 && upB!=0) hIn->Fit(fTOF,"N","",lowB,upB);
        else cout << "Fit Aborted" << endl;
    }
    //else fTOF->Draw("same");
    fTOF->Draw("same");
    //c1->Update();
    return fTOF;
}

TF1* generateResponse(TH1F *hIn, Bool_t kFit=false){

    double t0;
    hIn->GetListOfFunctions()->Clear();
    //double yMax = hIn->GetBinContent(hIn->GetMaximumBin());
    //double upperX = hIn->GetBinCenter(hIn->GetNbinsX());
    TF1 *fTOF = new TF1("fTOF",nResponse,0,1000,11);
    fTOF->SetNpx(1000);
    fTOF->SetParNames("t0","a0","sig0","sig1","k1","d1","k2","d2","k3","d3","AA");
    //fTOF->SetParameter(0,xMax);
    //fTOF->SetParameter(10,yMax);
    TCanvas *c1 = new TCanvas();
    hIn->Draw();
    c1->Update();

    cout << "Peak t0? (enter 0 for exit)" << endl;
    cin >> t0;
        if (t0<=0) return NULL;
    cout << "Fitting Response around " << t0 << endl;

    double a0 = 0.1667 + (-0.00141)*t0 + (5.27e-6)*t0*t0;
    double sig0 = 0.2594 + (0.000699)*t0 + 0.000202*t0*t0;
    double sig1 = -0.139 + 0.0242*t0;
    double k1 = 0.1819 - 0.000118*t0 + (4.215e-6)*t0*t0;
    double d1 = 75.41/(t0+17.70)+1.878;
    double k2 = 0.0162 - (4.797e-5)*t0 + (1.887e-6)*t0*t0;
    double d2 = 8.684 + 0.1331*t0 - (3.685e-4)*t0*t0;
    double k3 = 0.00335 + (2.108e-4)*t0 - (6.485e-7)*t0*t0;
    double d3 = 320.057 - 2.324*t0 + 0.00441*t0*t0;

    double AA = hIn->GetBinContent(hIn->FindBin(t0));
    hIn->GetXaxis()->UnZoom();
    fTOF->SetParameters(t0,a0,sig0,sig1,k1,d1,k2,d2,k3,d3,AA);
    double lowB, upB;
    if(kFit){
        cout << "Fitting Range [lower upper]:";
        cin >> lowB >> upB;
        if(lowB<upB && lowB!=0 && upB!=0) hIn->Fit(fTOF,"N","",lowB,upB);
        else cout << "Fit Aborted" << endl;
    }
    //else fTOF->Draw("same");
    fTOF->Draw("same");
    //c1->Update();
    return fTOF;
}