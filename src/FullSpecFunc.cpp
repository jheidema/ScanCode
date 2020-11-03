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
        if(kVerbose) {cout << "[FULLSPECFUNC::ADDFUNCS] >>"; for (int i=0;i<ftext.at(iff).size();i++) cout << " " << ftext.at(iff).at(i).c_str(); cout << endl;}
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

void FullSpecFunc::GenerateSpecFunc(TH1D* hIn, bool kPrint){
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    hIn->GetListOfFunctions()->Clear();
    
    fB = BkgdFunc(hIn,false);
    bArray = fB->GetParameters();
    nBP = fB->GetNpar();
    if(kVerbose) cout << nBP << " background parameters\n";
    tf.SetBGParams(nBP,bArray);
    
    cout << "Opening " << filename << endl;
    if(kGS && kVerbose) cout << "Adding GS Transitions" << endl;
    
    sf.SetVerbose(kVerbose);
    TF1 *fA = sf.CreateFunc(filename,0.0,kGS);
    if(kVerbose&&kGS) sf.PrintExBR();

    
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

TF1* FullSpecFunc::GenerateFitFunc(TH1D* hIn, bool kGNFit){
    
    GenerateSpecFunc(hIn);
    double binScale = hIn->GetBinWidth(10)/0.750;
    if(kVerbose&&binScale!=1) cout << "Adjusting amplitude for different bin width. [Scale=" << binScale << "]" << endl;

    gf.SetBGParams(nBP,bArray);
    AddFuncs(gf,funcfilename);        
    
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
    double tgate = 0.250;
    double fratio = 2.0;

    if(kVerbose && kFloatGS) cout << "TOF Gate: " << tgate << "\tFloat Gate: " << tgate/fratio << "\n"; 

    for (int is=0; is < np; is++) 
        fFit->FixParameter(is,0);  //Fix parameters first so subsequent peak parameters aren't used
    
    for (int is=0; is < nfc; is++){
        //Adding Gamma Gated Neutron Responses
        if (is<nF) {

            fFit->FixParameter(2*is,tf.GetParam(2*(is)));
            if(!kGNFit){ 
                fFit->FixParameter(2*is+1,tf.GetParam(2*(is)+1)*binScale);
                if(kVerbose) cout << "Fixing Gamma Nu peak: " << tf.GetParam(2*(is)) << " " << tf.GetParam(2*(is)+1)*binScale << "\n";
            }
            else {
                //fFit->SetParameter(2*is,tf.GetParam(2*(is)));
                //fFit->SetParLimits(2*is,tf.GetParam(2*(is))-tgate/fratio,tf.GetParam(2*(is))+tgate/fratio);
                fFit->SetParameter(2*is+1,tf.GetParam(2*(is)+1)*binScale);
                fFit->SetParLimits(2*is+1,tf.GetParam(2*(is)+1)*binScale*0.5,tf.GetParam(2*(is)+1)*binScale*2.0);
                if(kVerbose) cout << "Setting Gamma Nu peak: " << tf.GetParam(2*(is)) << " " << tf.GetParam(2*(is)+1)*binScale << "\n";
            }
            fFit->SetParName(2*is,Form("fGN%d_t0",is));
            fFit->SetParName(2*is+1,Form("fGN%d_A",is));
        }
        //Adding Ground State Transitions calculated from Gamma Gated Levels
        else if (is>=nF&&is<nF+nGS) {
            
            if((is-nF)==0) 
                rlo = gsInfo.at(is-nF).first-5;
            
            tnew  = gsInfo.at(is-nF).first;
            double an = gsInfo.at(is-nF).second;
            
            if(an>0.1 && (tnew-told)>tgate){
                fFit->SetParameter(2*is+1,an*binScale); //Float amplitude
                //fFit->SetParameter(2*is+1,200.*binScale); //Float amplitude
                fFit->SetParLimits(2*is+1,0.,1000.);
                
                if(kFloatGS&&(tnew>41.||tnew<36.)){
                //if(kFloatGS){
                    fFit->SetParameter(2*is,tnew);
                    fFit->SetParLimits(2*is,tnew-tgate/fratio,tnew+tgate/fratio);
                    if(kVerbose) cout << "Setting GS Peak: " << tnew << " " << fFit->GetParameter(2*is+1) << "\n";
                } else{
                    fFit->FixParameter(2*is,tnew); //Fix TOF
                    if(kVerbose) cout << "Fixing GS Peak: " << tnew << " " << fFit->GetParameter(2*is+1) << "\n";
                }
                
                rhi  = tnew+7.5;
                //hIn->Fit(fFit,"QN","",rlo,rhi);
                told = fFit->GetParameter(2*is);
            } else{ 
                fFit->FixParameter(2*is,tnew); //Fix TOF
                fFit->FixParameter(2*is+1,0.);
            }
        
            fFit->SetParName(2*is,Form("fGS%d_t0",is-nF));
            fFit->SetParName(2*is+1,Form("fGS%d_A",is-nF));

        }
        //Adding additional neutron responses from input files
        else if (is>=nF+nGS){
            double tof=gf.n_tofs.at(is-(nF+nGS));
                if (tof>tnew) tnew=tof;
            double n_amp=gf.n_amps.at(is-(nF+nGS));
            
            fFit->SetParameter(2*is,tof);
            fFit->SetParLimits(2*is,tof*0.985,tof*1.015);
            fFit->SetParameter(2*is+1,n_amp*binScale);
            fFit->SetParLimits(2*is+1,0.,1000*binScale);

            fFit->SetParName(2*is,Form("fNF%d_t0",is-(nF+nGS)));
            fFit->SetParName(2*is+1,Form("fNF%d_A",is-(nF+nGS)));
            
            if(kVerbose) cout << "Setting Added Peak: " << tof << " " << n_amp << "\n";
        }
    }
    rhi = tnew+7.5;

    TFitResultPtr r = hIn->Fit(fFit,"QNS","",rlo,rhi);
    if(kVerbose&&!kFitTail) r->Print();
    if(kFitTail){
        for(int i=0;i<np;i=i+2) fFit->FixParameter(i,fFit->GetParameter(i));
        TFitResultPtr r = hIn->Fit(fFit,"QNS","",15,500);
        if(kVerbose) r->Print();
    }
    
    
    return fFit;
}
#endif
