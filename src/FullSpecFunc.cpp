#include "FullSpecFunc.hpp"

#ifdef FullSpecFunc_hpp

#include "TH1F.h"
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"

#include "BkgdFunc.hpp"
#include "tofFuncClass.hpp"

using namespace std;

FullSpecFunc::FullSpecFunc(){};

FullSpecFunc::FullSpecFunc(bool kVerbose_){
    kVerbose = kVerbose_;
};


FullSpecFunc::~FullSpecFunc(){};

bool FullSpecFunc::OpenFile(){
    fr.OpenFile(filename);  
}

void FullSpecFunc::AddFuncs(FitFuncClass &gff, const char *filename_){
    
    if(!(funcf.OpenFile(filename_))) return;
    
    funcf.SetVerbose(kVerbose);

    vector<vector<string>> ftext = funcf.LoadFile();
    int nfs = ftext.size();
    if (nfs==0) return;

    for (int iff=0; iff<nfs; iff++){
        string funcname = ftext.at(iff).at(0);
        if(funcname=="gaus" || funcname=="gauss"){
            gff.AddGaus(stod(ftext.at(iff).at(1)),stod(ftext.at(iff).at(2)),stod(ftext.at(iff).at(3)));
            ngs++;
        }
        else if(funcname=="landau"){
            gff.AddLandau(stod(ftext.at(iff).at(1)),stod(ftext.at(iff).at(2)),stod(ftext.at(iff).at(3)));
            nld++;
        }
        else if(funcname=="nufunc" || funcname=="nu"){
            gff.AddNuFunc(stod(ftext.at(iff).at(1)),stod(ftext.at(iff).at(2)));
            nnu++;
        }
    }
    return;
}

void FullSpecFunc::GenerateSpecFunc(TH1D* hIn, bool kHyp){
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    hIn->GetListOfFunctions()->Clear();
    
    fB = BkgdFunc(hIn,kHyp);
    bArray = fB->GetParameters();
    nBP = fB->GetNpar();
    if(kVerbose) cout << nBP << " background parameters\n";
    tf.SetBGParams(nBP,bArray);
    
    cout << "Opening " << filename << endl;
    if(kGS && kVerbose) cout << "Adding GS Transitions" << endl;
    
    sf.SetVerbose(kVerbose);
    TF1 *fA = sf.CreateFunc(filename,0.0,kGS);
    if(kVerbose) sf.PrintExBR();

    
    nFuncs = fA->GetParameter(0);

    for (int iF=0; iF<nFuncs; iF++){
        double t = fA->GetParameter(iF*nrp+1);
        double a = fA->GetParameter(iF*nrp+2);
        tf.InsertFunction(t,a);
    }    

    fOut = new TF1("fOut",tf,0,800,0);
    fOut->SetNpx(2000); 

    return;
}

TF1* FullSpecFunc::GenerateFitFunc(TH1D* hIn){
    
    GenerateSpecFunc(hIn);

    gf.SetBGParams(nBP,bArray);
    AddFuncs(gf,funcfilename);        
    //if(kVerbose) cout << "nLandau: " << gf.GetNLand() << endl;

    int nF = tf.GetNFuncs();
    
    int nN = gf.GetNNu();

    std::vector< std::pair<double, double> > gsInfo = sf.GetGSInfo();
    int nGS = gsInfo.size();
    
    int nfc = nF+nGS+nN;
    int np = nfc*2;
    gf.SetNFuncs(nfc);
    
    if(kVerbose){
        cout << np << " Parameters" << endl;
        cout << "Gamma Gated: " << nF*2 << "\t GS: " << nGS*2 << endl;
    }

    TF1 *fFit = new TF1("fFit",gf,0,800,np);
    double rlo,rhi;
    double told=0.0;
    double tnew=0.0;
    double tgate = 0.500;
    for (int is=0; is < np; is++) fFit->FixParameter(is,0);  //Fix parameters first so subsequent peak parameters aren't used
    
    for (int is=0; is < nfc; is++){
        if (is<nF) {
            fFit->FixParameter(2*is,tf.GetParam(2*(is)));
            fFit->FixParameter(2*is+1,tf.GetParam(2*(is)+1));
            if(kVerbose) cout << "Fixing peak: " << tf.GetParam(2*(is)) << " " << tf.GetParam(2*(is)+1) << "\n";
        }
        else if (is>=nF&&is<nF+nGS) {
            if((is-nF)==0) rlo = gsInfo.at(is-nF).first-5;
            //gf.SetNFuncs(is+1);
            tnew  = gsInfo.at(is-nF).first;
            double an = gsInfo.at(is-nF).second;
            
            fFit->FixParameter(2*is,tnew); //Fix TOF
            
            if(an>1 && (tnew-told)>tgate){
                fFit->SetParameter(2*is+1,an); //Float amplitude
                fFit->SetParLimits(2*is+1,0.001,1000.);
                told = tnew;
                rhi  = tnew+10;
                hIn->Fit(fFit,"QN","",rlo,rhi);
            } else fFit->FixParameter(2*is+1,0.);
            if(kVerbose) cout << "Setting GS Peak: " << tnew << " " << fFit->GetParameter(2*is+1) << "\n";
        }
        else if (is>=nF+nGS){
            double tof=gf.n_tofs.at(is-(nF+nGS));
                if (tof>tnew) tnew=tof;
            double n_amp=gf.n_amps.at(is-(nF+nGS));
            
            fFit->SetParameter(2*is,tof);
            fFit->SetParLimits(2*is,tof*0.97,tof*1.03);
            fFit->SetParLimits(2*is+1,0.00001,1000);
            fFit->SetParameter(2*is+1,n_amp);
            
            if(kVerbose) cout << "Setting Added Peak: " << tof << " " << n_amp << "\n";
        }
    }
    rhi = tnew+20;

    if(kFitTail){
        TFitResultPtr r = hIn->Fit(fFit,"QNS","",25,500);
        if(kVerbose) r->Print();
    }
    else{
        if(kVerbose) hIn->Fit(fFit,"LN","",rlo,rhi); 
        else hIn->Fit(fFit,"LQN","",rlo,rhi);
    }
    
    return fFit;
}
#endif