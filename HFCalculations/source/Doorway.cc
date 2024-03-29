#include <iostream>
#include <map>

#include "HFReader.hpp"
#include "FileReader.hpp"
#include "VandleEff.hpp"
#include "dummyFuncs.hpp"
#include "optionHandler.hpp"

#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TGaxis.h"
#include "TAttFill.h"
#include "TLegend.h"
#include "TH2F.h"
#include "TH1I.h"
#include "TF1.h"
#include "TFile.h"
#include "TColor.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TStyle.h"
#include "TROOT.h"

using namespace std;

bool kVerbose;
bool kErr;
double dx = 0.05;
int fThresh;
double thresh = 1E-5;
int cType=0;
double minE=4, maxE=10;
bool gShade=false;

double chi2Calc( vector<pair<double,pair<double,double>>> pIn);
double chi2Err( vector<pair<double,pair<double,double>>> pIn);

double calcTOF(double E){
    return 100.0/29.98*sqrt(938.9/(2*E));
}

double calcErr(double E){
    double tof = calcTOF(E);
    return sqrt(pow(2*1.0/tof,2)+pow(2*0.03/1,2))*E;
}

double BreitWigner(double x, double *p){
    double xx = x;
    
    //double gm=0.25*p[1]*p[1]/(0.25*pow(p[1],2)+pow(p[0]-xx,2));
    double gm=0.25*p[1]/(0.25*pow(p[1],2)+pow(p[0]-xx,2));

    return gm;
}

class DWfunc {
    public:
        unsigned int nF=0;
        vector<double> gam;
        vector <double> en;
        vector <double> c2S;

    DWfunc(){};
    ~DWfunc(){};

    void Clear() {nF=0; gam.clear(); en.clear(); c2S.clear();};
    void InsertFunction(double p1, double p2, double p3){
        en.push_back(p1);
        gam.push_back(p2);
        c2S.push_back(p3);
        nF++;
    };
    unsigned int GetNF(){return nF;};

    double operator() (double x){
        
        double result = 0.0;
        double par[2];
        for (int i=0;i<nF;i++){
            par[0] = en.at(i);
            par[1] = gam.at(i)*c2S.at(i);
            result += BreitWigner(x,par);
            //if (result>0.1) result=0.1;
        }
    return result;
    }
    double operator() (double *x, double *p){
        
        double result = 0.0;
        double par[2];
        for (int i=0;i<nF;i++){
            par[0] = en.at(i);
            par[1] = gam.at(i)*c2S.at(i);
            result += BreitWigner(x[0],par);
            if (result>0.1) result=0.1;
        }
    return result;
    }

};

const int nstates=3;
int lwidth=2;

int mstyle[nstates] = {20,21,22}; // filled markers
//int mstyle[nstates] = {24,25,26}; // hollow markers
//int mstyle[nstates] = {53,54,55}; // hollow markers, thick line
int mcolor[nstates] = {1,1,1};
int mfill[nstates] = {kCyan+4,kCyan-4,kCyan-2};
int spins[nstates] = {7,3,9};

void doorway(const char *filename_){

    if(minE>maxE) {cout << "Invalid Energy Range. MinE must be less than MaxE." << endl; return;}
    cout << "Setting SF threshold to " << thresh << endl;
    
    //gStyle->SetEndErrorSize(8.5);
    gStyle->SetOptStat(0);
    gStyle->SetEndErrorSize(6);

    FileReader bgtF("BGT.tsv");
    bgtF.SetVerbose(kVerbose);
    vector<vector<string>> bgtS = bgtF.LoadFile();
    int nrow = bgtS.size();
    int ncol = bgtS.at(0).size();
    
    TGraphAsymmErrors *gdata[3];
    
    int fc[3] = {kCyan+3,kCyan-7,kCyan-3};

    for (int i=0; i<nstates;i++){
        
        gdata[i] = new TGraphAsymmErrors();
        gdata[i]->SetLineColor(mcolor[i]);
        gdata[i]->SetMarkerColor(mcolor[i]);
        gdata[i]->SetMarkerStyle(mstyle[i]);
        gdata[i]->SetFillColor(mfill[i]);
        if(gShade) gdata[i]->SetFillColorAlpha(mfill[i],0.75);
        gdata[i]->SetMarkerSize(0.85);
        gdata[i]->SetLineWidth(lwidth-1);
    }

    double btemp; 

    for (int i=0;i<nrow;i++){
        double Ex = stod(bgtS.at(i).at(0));
        double bGS = stod(bgtS.at(i).at(1));
        double b1561 = stod(bgtS.at(i).at(3));
        double b854 = stod(bgtS.at(i).at(5));
        double bAll = bGS+b1561+b854;

        double eGS = stod(bgtS.at(i).at(2));
        double e1561 = stod(bgtS.at(i).at(4));
        double e854 = stod(bgtS.at(i).at(6));

        double err = calcErr(-1.*(-Ex+3.62+1.561));

        if(bGS>0.0)  { 
            gdata[0]->SetPoint(i,-Ex,bGS/bAll); 
            gdata[0]->SetPointEYhigh(i,(bGS+eGS)/(bAll+eGS)-bGS/bAll); 
            gdata[0]->SetPointEYlow(i,bGS/bAll-(bGS-eGS)/(bAll-eGS)); 
            gdata[0]->SetPointEXhigh(i,err/2.);
            gdata[0]->SetPointEXlow(i,err/2.);
        }
        if(b854>0.0) {
            btemp=bGS+b854; 
            gdata[1]->SetPoint(i,-Ex,btemp/bAll); 
            gdata[1]->SetPointEYhigh(i,(b854+e854)/(bAll+e854)-b854/bAll); 
            gdata[1]->SetPointEYlow(i,b854/bAll-(b854-e854)/(bAll-e854));
            gdata[1]->SetPointEXhigh(i,err/2.);
            gdata[1]->SetPointEXlow(i,err/2.); 
        }
        if(b1561>0.0) {
            btemp=bGS+b854+b1561; 
            gdata[2]->SetPoint(i,-Ex,btemp/bAll); 
            gdata[2]->SetPointEYhigh(i,(b1561+e1561)/(bAll+e1561)-b1561/bAll); 
            gdata[2]->SetPointEYlow(i,b1561/bAll-(b1561-e1561)/(bAll-e1561));
            gdata[2]->SetPointEXhigh(i,err/2.);
            gdata[2]->SetPointEXlow(i,err/2.); 
        }
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

    TGraph *gcalc[nFiles][3];

    TH1F *hrel[nFiles][3];
    TH1F *habs[nFiles][3];
    
    int lc[3] = {kCyan-9,kGreen-9,kRed-9};
    int ld[3] = {kAzure+5,kCyan-9,kTeal-1};
    //int ld[3] = {kGreen+3,kGreen-8,kGreen-5};
    //int ld[3] = {kBlue,kGreen+3,kRed+2};
    
    TF1 *ff[3];

    THStack *hs[nFiles];
    THStack *ha[nFiles];
    int spin;

    //ofstream outF;

    double lX = minE;
    double hX = maxE;
    
    int nBins = (hX-lX)/dx;
    double itg[4];
    double gams[4] = {0,0.854,1.561,2.004};
    double t_itg;

    for (int f=0;f<nFiles;f++){
        npeaks=0;
        if(fr.IsOpen()) fr.Reset();

        fr.SetFileName(fList.at(f).at(0).c_str());
        fr.OpenFile();
    
        
        if(fList.at(f).size()>1) spin = stoi(fList.at(f).at(1));
        else spin=f;
        //int N7=0, N3=0, N9=0;
        fgs.Clear(); f3.Clear(); f5.Clear(); f9.Clear();
        
        hs[f] = new THStack(Form("hJ%d",spin),Form("J %d-",spin));
        ha[f] = new THStack(Form("hJ%d",spin),Form("J %d-",spin));
        
        //outF.open(Form("output/TSV/Doorway_J%dm.tsv",spin));
        //outF << spin << "\t7/2\t3/2\t9/2" << endl;
        //outF << std::fixed ;

        while(!fr.EoF()){
        vector<string> finfo = fr.GetRow();
        int nCols = finfo.size();
        if (nCols==0) continue;

        double width=stod(finfo.at(8));
        double cS=stod(finfo.at(7));
        double Ex = stod(finfo.at(1));
        int ff = (int)(stod(finfo.at(4))*2);
        
	    //if (cS==0.0) cS = 0.0000049;
        if (cS==0) {cout << "cS2 error" << endl; cS = 0.00001;}
         if(width*cS>thresh) {
         //if(Ex<8.0){    
         // cout << "Number of Functions: 7/2: " << fgs.GetNF() << ", 3/2: " << f3.GetNF() << ", 9/2: " << f9.GetNF() << endl;
          switch(ff){
             case(3):
             f3.InsertFunction(Ex,width,cS);
             break;
             
             case(5):
             f5.InsertFunction(Ex,width,cS);
             break;
             
             case(7):
             fgs.InsertFunction(Ex,width,cS);
             break;
             
             case(9):
             f9.InsertFunction(Ex,width,cS);
             break;
             
             default: 
             if(kVerbose) cout << "Unknown spin state populated from Doorway File" << endl;
         }   
        }
        //}
        }
    
        unsigned int ngs = fgs.nF;
        unsigned int n3 =  f3.nF;
        unsigned int n5 =  f5.nF;
        unsigned int n9 =  f9.nF;

        if (kVerbose) cout << "Number of Functions: 7/2: " << ngs << ", 3/2: " << n3 << ", 9/2: " << n9 << endl;
        //cout << "Number of Functions: 7/2: " << fgs.GetNF() << ", 3/2: " << f3.GetNF() << ", 9/2: " << f9.GetNF() << endl;

        for(int k=0;k<3;k++) {
            hrel[f][k] = new TH1F(Form("h%d_j%d",k,spin),Form("h%d_j%d",k,spin),nBins,-hX,-lX);
            if(gShade) hrel[f][k]->SetFillColorAlpha(ld[k],0.65); 
            hrel[f][k]->SetLineColor(ld[k]);

            gcalc[f][k] = new TGraph();
            gcalc[f][k]->SetName(Form("g%d_s%d",k,spin));
            gcalc[f][k]->SetLineColor(ld[k]);
            gcalc[f][k]->SetLineWidth(1);
            gcalc[f][k]->SetFillColor(ld[k]);
            //if(gShade) gcalc[f][k]->SetFillColorAlpha(ld[k],0.25);
            gcalc[f][k]->SetPoint(0,-minE,0);

            habs[f][k] = new TH1F(Form("habs%d_j%d",k,spin),Form("habs%d_j%d",k,spin),nBins,-hX,-lX);
            habs[f][k]->SetFillColor(ld[k]); 
            habs[f][k]->SetLineColor(ld[k]);
        
        }

        double xlo;
        double xhi;
        double xmid;
        
        for(int i=1;i<=nBins;i++){
            xlo = lX+dx*(i-1);
            xhi = xlo+dx;
            xmid = xlo+dx/2.0;
            //xmid = lX+dx*(i-1);
            
            if(xmid>(gams[0]+3.62)){
                //itg[0] = fgs(xmid);
                itg[0] = (fgs(xlo)+fgs(xmid)+fgs(xhi))*dx/3.0;
                if(xmid>(gams[1]+3.62)){
                    //itg[1] = f3(xmid); 
                    itg[1] = (f3(xlo)+f3(xmid)+f3(xhi))*dx/3.0;
                    if(xmid>(gams[2]+3.62)){
                        //itg[2] = f9(xmid); 
                        itg[2] = (f9(xlo)+f9(xmid)+f9(xhi))*dx/3.0;
                        if(xmid>(gams[3]+3.62))
                            //itg[3] = f5(xmid); 
                            itg[3] = (f5(xlo)+f5(xmid)+f5(xhi))*dx/3.0;
                    }
                }
            }
            //outF << std::setprecision(4) << hrel[f][0]->GetBinCenter(i); 
            t_itg = itg[0]+itg[1]+itg[2];
            double itg_temp = 0;
            for (int k=0;k<3;k++){ 
                itg_temp += itg[k];
                gcalc[f][k]->SetPoint(i,-xmid,itg_temp/t_itg);
                hrel[f][k]->SetBinContent(hrel[f][k]->FindBin(-xmid),itg[k]/t_itg); habs[f][k]->SetBinContent(habs[f][k]->FindBin(-xmid),itg[k]);
                //if(itg[k]>0.0) outF << std::setprecision(8) << "\t" << itg[k];
                //else outF << std::setprecision(8) << "\t" << 0.00000001;
                itg[k]=0.0;
            }
            //outF << endl;
        }
            for(int k=0;k<3;k++)
                gcalc[f][k]->SetPoint(nBins+1,-xmid,0);
            //outF.close();
    }

    ff[0] = new TF1("f0",fgs,4,10,0); ff[0]->SetLineColor(lc[0]); ff[0]->SetNpx(5000);
    ff[1] = new TF1("f1",f3,4,10,0);  ff[1]->SetLineColor(lc[1]); ff[1]->SetNpx(5000);
    ff[2] = new TF1("f2",f9,4,10,0);  ff[2]->SetLineColor(lc[2]); ff[2]->SetNpx(5000);

    if (kVerbose) cout << "Stacking Histograms..." << endl;

    TCanvas *c1 = new TCanvas("c1","c1",1000,700);

    TFile *outF = new TFile("output/files/Doorway.root","RECREATE");

    for (int i=0;i<nFiles;i++){
        for(int j=0;j<nstates;j++){
            gcalc[i][j]->Write();
        }
    }
    
    outF->Close();

    if(cType==1){

        if(nFiles<3) c1->Divide(1,nFiles);
        else c1->Divide(2,ceil(nFiles/2.));

        for(int i=0;i<nFiles;i++){
            
            c1->cd(i+1);
            c1->GetPad(i+1)->SetFrameLineColor(kWhite);
            
            gcalc[i][2]->GetXaxis()->SetLimits(-maxE,-minE);
            gcalc[i][2]->GetXaxis()->SetRangeUser(-maxE,-minE);
            gcalc[i][2]->GetXaxis()->SetLabelSize(0.06);
            gcalc[i][2]->GetXaxis()->SetLabelOffset(0.05);
            gcalc[i][2]->GetXaxis()->SetNdivisions(508);
            gcalc[i][2]->GetYaxis()->SetLabelSize(0.06);

            gcalc[i][2]->Draw("AF");
            gcalc[i][1]->Draw("F same");
            gcalc[i][0]->Draw("F same");

            //for(int j=0;j<nstates;j++){
            //    gdata[j]->Draw("P5 same");
            //}
        }

        c1->SaveAs("output/canvases/Doorway_Rel.root");
        c1->SaveAs("output/pdfs/Doorway_Rel.pdf");
       
    }
    else if (cType==2){

        c1->Divide(1,2);
        
        TLegend *ll[nFiles];
        
        for(int i=0;i<nFiles;i++){

            ll[i] = new TLegend(0.4,0.9,0.6,1.0);
            ll[i]->SetNColumns(3);

            c1->cd(1);
            c1->GetPad(1)->SetFrameLineColor(kWhite);
            
            gcalc[i][2]->GetXaxis()->SetLimits(-maxE,-minE);
            gcalc[i][2]->GetXaxis()->SetRangeUser(-maxE,-minE);
            gcalc[i][2]->GetXaxis()->SetLabelSize(0.06);
            gcalc[i][2]->GetXaxis()->SetNdivisions(508);
            gcalc[i][2]->GetXaxis()->SetLabelOffset(0.05);
            gcalc[i][2]->GetXaxis()->SetLabelColor(kWhite);
            //gcalc[i][2]->GetXaxis()->SetAxisColor(kWhite);
            gcalc[i][2]->GetYaxis()->SetLabelSize(0.06);
            gcalc[i][2]->GetYaxis()->SetLabelColor(kWhite);
            //gcalc[i][2]->GetYaxis()->SetAxisColor(kWhite);
            gcalc[i][2]->GetYaxis()->SetRangeUser(0,1.3);

            gcalc[i][2]->Draw("AF");
            gcalc[i][1]->Draw("F same");
            gcalc[i][0]->Draw("F same");

            for(int j=0;j<nstates;j++){
                ll[i]->AddEntry(gcalc[i][j],Form("%d/2-",spins[j]),"f");
            //    gdata[j]->Draw("P5 same");
            }
            ll[i]->Draw();
            
            gPad->RedrawAxis();
            
            //TGaxis *xx = new TGaxis(-8.5,0,-6,0,-8.5,-6,508);
            //xx->SetLabelSize(0.06);
            //xx->SetNdivisions(508);
            //xx->SetLabelOffset(0.05);

            //TGaxis *yy = new TGaxis(-8.5,0,-8.5,1.3,0,1.3,510,"S");
            //yy->SetLabelSize(0.06);
            //yy->SetTickLength(0.01);

            //xx->Draw();
            //yy->Draw();


            c1->cd(2);
            c1->GetPad(2)->SetLogy();
            c1->GetPad(2)->SetFrameLineColor(kWhite);
            
            habs[i][0]->Draw();
            habs[i][1]->Draw("same");
            habs[i][2]->Draw("same");

            
            c1->SaveAs(Form("output/canvases/Doorway_%s.root",hs[i]->GetName()));
            c1->SaveAs(Form("output/pdfs/Doorway_%s.pdf",hs[i]->GetName()));
            
        }
    }
    
    TFile *fOut = new TFile("output/hFile.root","RECREATE");
    c1->Write();
    for (int i=0;i<nFiles;i++)
        for (int k=0;k<3;k++){
            //hrel[i][k]->Write();
            habs[i][k]->Write();
            if(i==0) ff[k]->Write();
        }
    fOut->Close();

    cout << "Done Loading Files" << endl;
    fr.Reset();
    return;

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
  handler.add(optionExt("delta",required_argument, NULL,'D',"","Set Histogram Bin Width in MeV (Must be able to factor 6 MeV)"));
  handler.add(optionExt("err",optional_argument, NULL,'X',"","Calculate Chi2 Fit Error"));
  handler.add(optionExt("thresh",required_argument, NULL,'T',"","Set Threshold for Spectroscopic Factors"));
  handler.add(optionExt("canvas",required_argument, NULL,'c',"","Differentiate between canvas styles for histograms.\n                                          (1) Relative and Absolute stacked histograms on separate canvases.\n                                          (2) Relative and Absolute hists on same canvas for each level"));
  handler.add(optionExt("minE",required_argument, NULL,'e',"","Minimum energy to plot"));
  handler.add(optionExt("maxE",required_argument, NULL,'E',"","Maximum energy to plot"));
  handler.add(optionExt("shade",optional_argument, NULL,'S',"","Make plot fills transparent"));
  

  
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
    dx = stod(handler.getOption(1)->argument);
    cout << "Setting Bin Width to " << dx << " MeV" << endl;
  }

  if(handler.getOption(2)->active){
    cout << "Chi2 Error being calculated" << endl;
    kErr = true;
  }else
    kErr = false;

  if(handler.getOption(3)->active){
    fThresh = stoi(handler.getOption(3)->argument);
    thresh = pow(10,-1.0*fThresh);
  }

  if(handler.getOption(4)->active){
    cType = stoi(handler.getOption(4)->argument);
    //gStyle->SetCanvasPreferGL(true);
    cout << "Setting Canvas Style " << cType << endl;
  }

  if(handler.getOption(5)->active){
    double _minE = stod(handler.getOption(5)->argument);
    if(_minE>4) 
        minE = _minE;
  }

  if(handler.getOption(6)->active){
    double _maxE = stod(handler.getOption(6)->argument);
    if(_maxE<10)
        maxE=_maxE;
  }

  if(handler.getOption(7)->active)
    gShade=true;

  const char* filename = argv[argc-1];
  doorway(filename);
   
 return 0;
}
