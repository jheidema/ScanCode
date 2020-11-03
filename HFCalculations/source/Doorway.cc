#include <iostream>
#include <map>

#include "HFReader.hpp"
#include "FileReader.hpp"
#include "VandleEff.hpp"
#include "dummyFuncs.hpp"
#include "optionHandler.hpp"

#include "TGraphErrors.h"
#include "TH2F.h"
#include "TH1I.h"
#include "TFile.h"
#include "TColor.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TStyle.h"
#include "TROOT.h"

using namespace std;

bool kVerbose;
bool kTrend;
bool kErr;

double chi2Calc( vector<pair<double,pair<double,double>>> pIn);
double chi2Err( vector<pair<double,pair<double,double>>> pIn);

double BreitWigner(double x, double *p){
    double xx = x;
    
    double gm=0.5*p[1]/(0.25*pow(p[1],2)+pow(p[0]-xx,2));
    
    return gm;
}

class DWfunc {
    public:
        unsigned int nF=0;
        vector<double> gam;
        vector <double> en;

    DWfunc(){};
    ~DWfunc(){};

    void Clear() {nF=0; gam.clear(); en.clear();};
    void InsertFunction(double p1, double p2){
        en.push_back(p1);
        gam.push_back(p2);
        nF++;
    };
    unsigned int GetNF(){return nF;};

    double operator() (double x){
        
        double result = 0.0;
        double par[2];
        for (int i=0;i<nF;i++){
            par[0] = en.at(i);
            par[1] = gam.at(i);
            result += BreitWigner(x,par);
        }
    return result;
    }


};


void doorway(const char *filename_){
    //gStyle->SetEndErrorSize(8.5);
    gStyle->SetOptStat(0);
    
    FileReader bgtF("BGT.tsv");
    bgtF.SetVerbose(kVerbose);
    vector<vector<string>> bgtS = bgtF.LoadFile();
    int nrow = bgtS.size();
    int ncol = bgtS.at(0).size();
    vector<vector<double>> bgt;
    vector<double> dummy;
    
    TGraphErrors *g1561 = new TGraphErrors(); g1561->SetName("g1561");
    TGraphErrors *g854 = new TGraphErrors(); g854->SetName("g854");
    TGraphErrors *gGS = new TGraphErrors(); gGS->SetName("gGS");

    gGS->SetMarkerStyle(8); g1561->SetMarkerStyle(8); g854->SetMarkerStyle(8);
    gGS->SetLineWidth(2); g1561->SetLineWidth(2); g854->SetLineWidth(2);
    gGS->SetLineColor(kBlue); g1561->SetLineColor(kRed); g854->SetLineColor(kGreen);
    gGS->SetMarkerColor(kBlue); g1561->SetMarkerColor(kRed); g854->SetMarkerColor(kGreen);

    for (int i=0;i<nrow;i++){
        double Ex = stod(bgtS.at(i).at(0));
        double bGS = stod(bgtS.at(i).at(1));
        double b1561 = stod(bgtS.at(i).at(3));
        double b854 = stod(bgtS.at(i).at(5));
        double bAll = bGS+b1561+b854;

        double eGS = stod(bgtS.at(i).at(2));
        double e1561 = stod(bgtS.at(i).at(4));
        double e854 = stod(bgtS.at(i).at(6));


        if(bGS>0.0) gGS->SetPoint(i,Ex,bGS/bAll); gGS->SetPointError(i,0.0,eGS/bAll);
        if(b1561>0.0) g1561->SetPoint(i,Ex,(bGS+b1561)/bAll); g1561->SetPointError(i,0.0,e1561/bAll);
        if(b854>0.0) g854->SetPoint(i,Ex,(bGS+b1561+b854)/bAll); g854->SetPointError(i,0.0,e854/bAll);

        for(int j=0;j<ncol;j++) dummy.push_back(stod(bgtS.at(i).at(j)));
        dummy.push_back(bAll);
        bgt.push_back(dummy);
        dummy.clear();
        
    }

    
    FileReader fl(filename_);
    fl.SetVerbose(kVerbose);
    vector< vector< string > > fList = fl.LoadFile();
    
    const unsigned int nFiles = fList.size();
    fl.CloseFile();
   
    FileReader fr;
    fr.SetVerbose(kVerbose);
    
    vector<double> states;
    int npeaks;

    DWfunc fgs;
    DWfunc f3;
    DWfunc f9;
    DWfunc f5;

    TH1F *h[nFiles][3];
    int lc[3] = {kCyan-9,kGreen-9,kRed-9};

    for (int f=0;f<nFiles;f++){
        npeaks=0;
        if(fr.IsOpen()) fr.Reset();

        fr.SetFileName(fList.at(f).at(0).c_str());
        fr.OpenFile();
    
        //int N7=0, N3=0, N9=0;

        while(!fr.EoF()){
        vector<string> finfo = fr.GetRow();
        int nCols = finfo.size();
        if (nCols==0) continue;

        double width=stod(finfo.at(7));
        double cS=stod(finfo.at(8));
        double Ex = stod(finfo.at(1));
        int ff = (int)(stod(finfo.at(4))*2);
        

        if(width*cS>1E-7) {
        // cout << "Number of Functions: 7/2: " << fgs.GetNF() << ", 3/2: " << f3.GetNF() << ", 9/2: " << f9.GetNF() << endl;
         switch(ff){
            case(3):
            f3.InsertFunction(Ex,width*cS);
            break;
            
            case(5):
            f5.InsertFunction(Ex,width*cS);
            break;

            case(7):
            fgs.InsertFunction(Ex,width*cS);
            break;
            
            case(9):
            f9.InsertFunction(Ex,width*cS);
            break;
            
            default: 
            if(kVerbose) cout << "Unknown spin state populated from Doorway File" << endl;
         }   

        }
        }
    
        unsigned int ngs = fgs.nF;
        unsigned int n3 =  f3.nF;
        unsigned int n5 =  f5.nF;
        unsigned int n9 =  f9.nF;

        if (kVerbose) cout << "Number of Functions: 7/2: " << ngs << ", 3/2: " << n3 << ", 9/2: " << n9 << endl;
        //cout << "Number of Functions: 7/2: " << fgs.GetNF() << ", 3/2: " << f3.GetNF() << ", 9/2: " << f9.GetNF() << endl;


        int lX = 4;
        int hX = 10;
        double dx = 50/5;
        int nBins = (hX-lX)*dx;

        double itg[4];
        double t_itg;
        for(int k=0;k<3;k++) {
            h[f][k] = new TH1F(Form("h%d_j%d",k,f),Form("h%d_j%d",k,f),nBins,lX,hX);
            h[f][k]->SetFillColor(lc[k]); 
            h[f][k]->SetLineColor(lc[k]);
        }
            for(int i=1;i<=nBins;i++){
                double xlo = 4+0.05*(i-1);
                double xhi = xlo+0.05;
                itg[0] = 0.5*(fgs(xlo)+fgs(xhi))*dx;
                itg[1] = 0.5*(f3(xlo)+f3(xhi))*dx;
                itg[2] = 0.5*(f9(xlo)+f9(xhi))*dx;
                itg[3] = 0.5*(f5(xlo)+f5(xhi))*dx;
                
                t_itg = itg[0]+itg[1]+itg[2];
                for (int k=0;k<3;k++) h[f][k]->SetBinContent(i,itg[k]/t_itg);
        }
        
        fgs.Clear(); f3.Clear(); f5.Clear(); f9.Clear();
    }

    
    THStack *hs[nFiles];
        for(int i=0;i<nFiles;i++) hs[i] = new THStack(Form("hJ%d",i),Form("J %d-",i));

        if (kVerbose) cout << "Stacking Histograms..." << endl;

    TCanvas *c1 = new TCanvas();
    c1->Divide(1,nFiles);
    for(int i=0;i<nFiles;i++){
        c1->cd(i+1);
        c1->GetPad(i+1)->SetFrameLineColor(kWhite);
        hs[i]->Add(h[i][0]); hs[i]->Add(h[i][2]); hs[i]->Add(h[i][1]);
        
        hs[i]->Draw();
        hs[i]->GetXaxis()->SetLabelSize(0.12);
        hs[i]->GetYaxis()->SetLabelSize(0.1);
        
        gGS->DrawClone("same P");
        g1561->DrawClone("same P");
        g854->DrawClone("same P");
    }

    c1->SaveAs("c1_test.root");

    TFile *fOut = new TFile("hFile.root","RECREATE");
    for (int i=0;i<nFiles;i++)
        for (int k=0;k<3;k++) h[i][k]->Write();

    fOut->Close();

    cout << "Done Loading Files" << endl;
    fr.Reset();
    return;

    /*
    //Make bin array for histograms
    const int nXbins = states.size();
    
    if (kVerbose) cout << "Stacking Histograms..." << endl;

    TCanvas *c1 = new TCanvas();
    c1->Divide(2,ceil(nSpins/2.));
    

    c1->SaveAs("c1_test.root");
    
    //double chi[nSpins];

    vector<pair<double,pair<double,double>>> GTPair;
    vector<pair<double,pair<double,double>>> FFPair;
    double xi2GT, xi2FF;

    //TFile *fOut = new TFile("SummaryHists.root","RECREATE");
    FILE *outF = fopen("chi2Vals.tsv","w");
    fprintf(outF,"Spin\tGT\tFF\n");
    
    for(int j=0; j<nSpins;j++){
        GTPair.clear();
        FFPair.clear();
        xi2GT=-9999; xi2FF=-9999;

        if(kVerbose) cout << "Calculating Chi2 for Spin J=" << j << endl;
        
        for(int i=0; i<bgt.size(); i++){
        double Ex=bgt.at(i).at(0);
        double bAll=bgt.at(i).at(7);
        int binID=hCalc[0][j]->FindBin(Ex);
        
        for(int k=0;k<3;k++){
            if(Ex>6.8&&Ex<8){
                if(bgt.at(i).at(2*k+1)>0.0)
                    GTPair.push_back(make_pair(hCalc[k][j]->GetBinContent(binID),make_pair(bgt.at(i).at(2*k+1)/bAll,bgt.at(i).at(2*k+2)/bgt.at(i).at(2*k+1))));
                
               
            }
            else{
                if(bgt.at(i).at(2*k+1)>0.0)
                    FFPair.push_back(make_pair(hCalc[k][j]->GetBinContent(binID),make_pair(bgt.at(i).at(2*k+1)/bAll,bgt.at(i).at(2*k+2)/bgt.at(i).at(2*k+1))));
                
                }
        }
        }
            if(kErr){
                if(GTPair.size()>0) xi2GT = chi2Err(GTPair);
                if(FFPair.size()>0) xi2FF = chi2Err(FFPair);
            }
            else{
                if(GTPair.size()>0) xi2GT = chi2Calc(GTPair);
                if(FFPair.size()>0) xi2FF = chi2Calc(FFPair);
            }
        fprintf(outF,"J%d\t%.2f\t%.2f\n", j, xi2GT,xi2FF);
        if(kVerbose) printf("GT X^2/Ndf: %.2f, FF X^2/Ndf: %.2f\n", j, xi2GT, xi2FF);
    }
    
    fclose(outF);

    return; */
}

double chi2Calc(vector<pair<double,pair<double,double>>> pIn){
    double retval=0;
    int ni=0;
    for(int i=0;i<pIn.size();i++){
        if(pIn.at(i).second.first==0||pIn.at(i).first==0) retval += 0.0;
        else {retval += pow(pIn.at(i).first-pIn.at(i).second.first,2)/abs(pIn.at(i).first); ni++;}
    }
    return retval/ni;

};

double chi2Err(vector<pair<double,pair<double,double>>> pIn){
    double retval;
    int ni=0;
    for(int i=0;i<pIn.size();i++){
        if(pIn.at(i).second.first==0||pIn.at(i).first==0) retval += 0.0;
        else {retval += pow(pIn.at(i).first-pIn.at(i).second.first,2)/abs(pIn.at(i).second.second); ni++;}
    }

    return retval/(ni-1);
}


int main(int argc, char **argv){
    
  gROOT->SetBatch(true);

  optionHandler handler;
  handler.add(optionExt("debug",optional_argument, NULL,'d',"","Turn on verbose output"));
  handler.add(optionExt("trend",optional_argument, NULL,'T',"","Scale data to measure chi2 of trend"));
  handler.add(optionExt("err",optional_argument, NULL,'E',"","Calculate Chi2 Fit Error"));
  
  if (argc<2) {cout << "Incorrect number of arguments. Usage:\n";
   handler.help(argv[0]); 
   return 1;
  }

  if(!handler.setup(argc, argv))
	  return 1;
    
  if(handler.getOption(0)->active){
    cout << "Verbose/Debug mode enabled" << endl;
    kVerbose = true;
  }else
    kVerbose = false;

  if(handler.getOption(1)->active){
    cout << "Chi2 for trend being calculated" << endl;
    kTrend = true;
  }else
    kTrend = false;

  if(handler.getOption(2)->active){
    cout << "Chi2 Error being calculated" << endl;
    kErr = true;
  }else
    kErr = false;


  const char* filename = argv[argc-1];
  doorway(filename);
   
 return 0;
}