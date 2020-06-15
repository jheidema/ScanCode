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
    if(kVerbose) cout << "nLandau: " << gf.GetNLand() << endl;

    int nF = tf.GetNFuncs();
    std::vector< std::pair<double, double> > gsInfo = sf.GetGSInfo();
    int nGS = gsInfo.size();
    int nfc = nF+nGS;
    int np = (nF+nGS)*2;
    gf.SetNFuncs(nfc);
    
    if(kVerbose){
        cout << np << " Parameters" << endl;
        cout << "Gamma Gated: " << nF*2 << "\t GS: " << nGS*2 << endl;
    }

    TF1 *fFit = new TF1("fFit",gf,0,800,np);
    double rlo,rhi;
    double told=0.0;
    double tnew=0.0;
    for (int is=0; is < np; is++) fFit->FixParameter(is,0);  //Fix parameters first so subsequent peak parameters aren't used
    
    for (int is=0; is < nfc; is++){
        if (is<nF) {
            fFit->FixParameter(2*is,tf.GetParam(2*(is)));
            fFit->FixParameter(2*is+1,tf.GetParam(2*(is)+1));
            if(kVerbose) cout << "Fixing peak: " << tf.GetParam(2*(is)) << " " << tf.GetParam(2*(is)+1) << "\n";
            }
        if (is>=nF) {
            if((is-nF)==0) rlo = gsInfo.at(is-nF).first-5;
            //gf.SetNFuncs(is+1);
            fFit->FixParameter(2*is,gsInfo.at(is-nF).first); //Fix TOF
            tnew  = gsInfo.at(is-nF).first;
            double an = gsInfo.at(is-nF).second;
            
            if(an>10 && (tnew-told)>0.750){
                fFit->SetParameter(2*is+1,an); //Float amplitude
                fFit->SetParLimits(2*is+1,0.001,1000.);
                told = tnew;
            } else fFit->FixParameter(2*is+1,an);
            
            if(kVerbose) cout << "Setting peak: " << gsInfo.at(is-nF).first << " " << gsInfo.at(is-nF).second << "\n";
            rhi  = gsInfo.at(is-nF).first+5;
            hIn->Fit(fFit,"QN","",rlo,rhi);
            
            //cout << fFit->GetNumberFreeParameters() << " " << fFit->GetNDF() << "\n" << rlo << " " << rhi << endl;
            }
    }
    
    //TFitResultPtr r = hIn->Fit(fFit,"NS","",25,200);
    //if(kVerbose) r->Print();

    return fFit;
}
#endif