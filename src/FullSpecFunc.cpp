#include "FullSpecFunc.hpp"

#ifdef FullSpecFunc_hpp

#include "TH1F.h"
#include "TF1.h"

#include "BkgdFunc.hpp"


using namespace std;

FullSpecFunc::FullSpecFunc(){};

FullSpecFunc::FullSpecFunc(bool kVerbose_){
    kVerbose = kVerbose_;
};


FullSpecFunc::~FullSpecFunc(){};

bool FullSpecFunc::OpenFile(){
    fr.OpenFile(filename);  
}

void FullSpecFunc::GenerateSpecFunc(TH1D* hIn, bool kHyp){
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    hIn->GetListOfFunctions()->Clear();
    
    fB = BkgdFunc(hIn,kHyp);
    bArray = fB->GetParameters();
    nBP = fB->GetNpar();
    cout << nBP << " background parameters\n";
    tf.SetBGParams(nBP,bArray);
    
    cout << "Opening " << filename << endl;
    if(kGS) cout << "Adding GS Transitions" << endl;
    
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

TF1* FullSpecFunc::FitGSStates(TH1D* hIn){
    //GenerateSpecFunc(hIn);
    
    FullFuncClass gf;
    gf.SetBGParams(nBP,bArray);
    gf.SetFitBool(true);
    
    int nF = tf.GetNFuncs();
    std::vector< std::pair<double, double> > gsInfo = sf.GetGSInfo();
    int nGS = gsInfo.size();
    int np = (nF+nGS)*2;
    gf.SetNFuncs(nF+nGS);
    /* cout << np << " Parameters" << endl;
    TF1 *fFit = new TF1("fFit",gf,0,800,10);
    for (int is=0; is < np; is++){
        if (is<nGS) {
            fFit->FixParameter(2*is,gsInfo.at(is).first); //Fix TOF
            fFit->SetParameter(2*is+1,gsInfo.at(is).second); //Float amplitude
            fFit->SetParLimits(2*is+1,0.01,100.);
            if(kVerbose) cout << "Setting peak: " << gsInfo.at(is).first << " " << gsInfo.at(is).second << "\n";
            }
        if (is>=nGS) {
            fFit->FixParameter(2*is,tf.GetParam(2*(is-nGS)));fFit->FixParameter(2*is+1,tf.GetParam(2*(is-nGS)+1));
            if(kVerbose) cout << "Fixing peak: " << tf.GetParam(2*(is-nGS)) << " " << tf.GetParam(2*(is-nGS)+1) << "\n";
            }
    }

    //np = fFit->GetNpar()/2;
    //for (int ip=0; ip<np; ip++) cout << fFit->GetParameter(2*ip) << " " << fFit->GetParameter(2*ip+1) << endl;

    hIn->Fit(fFit,"N","",20,350);
    hIn->Fit(fFit,"N","",20,350); */

    ///Hacked code to manually shift peaks to 
    TF1 *fFit = new TF1("fFit",gf,0,800,np);
    double val, tof;
    for (int is=0; is<nGS+nF; is++){
        if(is<nF){
        fFit->FixParameter(2*is,tf.GetParam(2*(is)));fFit->FixParameter(2*is+1,tf.GetParam(2*(is)+1));
        if(kVerbose) cout << "Fixing peak: " << tf.GetParam(2*(is)) << " " << tf.GetParam(2*(is)+1) << "\n";        
        }
        else if(is>=nF){
        tof = gsInfo.at(is-nF).first;
        val = fFit->Eval(tof);
        cout << tof << "\t" << hIn->GetBinContent(hIn->FindBin(tof)) << "\t" << val << "\t" << gsInfo.at(is-nF).second << "\n";
        }
    }

    return fFit;

}
#endif