#include <iostream>
#include <map>

#include "HFReader.hpp"
#include "FileReader.hpp"
#include "VandleEff.hpp"
#include "dummyFuncs.hpp"
#include "optionHandler.hpp"

#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TAttFill.h"
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

double chi2Calc( vector<pair<double,pair<double,double>>> pIn);
double chi2Err( vector<pair<double,pair<double,double>>> pIn);

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


void doorway(const char *filename_){
    //gStyle->SetEndErrorSize(8.5);
    gStyle->SetOptStat(0);
    gStyle->SetEndErrorSize(6);

    FileReader bgtF("BGT.tsv");
    bgtF.SetVerbose(kVerbose);
    vector<vector<string>> bgtS = bgtF.LoadFile();
    int nrow = bgtS.size();
    int ncol = bgtS.at(0).size();
    vector<vector<double>> bgt;
    vector<double> dummy;
    
    TGraphAsymmErrors *g1561 = new TGraphAsymmErrors(); g1561->SetName("g1561");
    TGraphAsymmErrors *g854 = new TGraphAsymmErrors(); g854->SetName("g854");
    TGraphAsymmErrors *gGS = new TGraphAsymmErrors(); gGS->SetName("gGS");


    int fc[3] = {kCyan+3,kCyan-7,kCyan-3};

    gGS->SetMarkerStyle(8); g1561->SetMarkerStyle(8); g854->SetMarkerStyle(8);
    gGS->SetLineWidth(2); g1561->SetLineWidth(2); g854->SetLineWidth(2);
    gGS->SetLineColor(kBlue); g1561->SetLineColor(kRed+2); g854->SetLineColor(kGreen+3);
    gGS->SetMarkerColor(kBlue); g1561->SetMarkerColor(kRed+2); g854->SetMarkerColor(kGreen+3);

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


        if(bGS>0.0)  { gGS->SetPoint(i,-Ex,bGS/bAll); 
            gGS->SetPointEYhigh(i,(bGS+eGS)/(bAll+eGS)-bGS/bAll); gGS->SetPointEYlow(i,bGS/bAll-(bGS-eGS)/(bAll-eGS)); 
        }
        if(b1561>0.0) {btemp=bGS+b854+b1561; g1561->SetPoint(i,-Ex,btemp/bAll); 
            g1561->SetPointEYhigh(i,(b1561+e1561)/(bAll+e1561)-b1561/bAll); g1561->SetPointEYlow(i,b1561/bAll-(b1561-e1561)/(bAll-e1561)); 
        }
        if(b854>0.0) {btemp=bGS+b854; g854->SetPoint(i,-Ex,btemp/bAll); 
            g854->SetPointEYhigh(i,(b854+e854)/(bAll+e854)-b854/bAll); g854->SetPointEYlow(i,b854/bAll-(b854-e854)/(bAll-e854)); 
        }

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

    TH1F *hrel[nFiles][3];
    TH1F *habs[nFiles][3];
    int lc[3] = {kCyan-9,kGreen-9,kRed-9};
    int ld[3] = {kCyan+3,kCyan-7,kCyan-3};
    //int ld[3] = {kBlue,kGreen+3,kRed+2};
    
    TF1 *ff[3];

    THStack *hs[nFiles];
    THStack *ha[nFiles];
    int spin;

    ofstream outF;

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
        
        outF.open(Form("output/TSV/Doorway_J%dm.tsv",spin));
        //outF << spin << "\t7/2\t3/2\t9/2" << endl;
        outF << std::fixed ;

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

        double lX = 6;
        double hX = 8.5;
        
        int nBins = (hX-lX)/dx;

        double itg[4];
        double gams[4] = {0,0.854,1.561,2.004};
        double t_itg;
        for(int k=0;k<3;k++) {
            hrel[f][k] = new TH1F(Form("h%d_j%d",k,spin),Form("h%d_j%d",k,spin),nBins,-hX,-lX);
            hrel[f][k]->SetFillColor(ld[k]); 
            hrel[f][k]->SetLineColor(ld[k]);

            habs[f][k] = new TH1F(Form("habs%d_j%d",k,spin),Form("habs%d_j%d",k,spin),nBins,-hX,-lX);
            habs[f][k]->SetFillColor(ld[k]); 
            habs[f][k]->SetLineColor(ld[k]);
        
        }
        for(int i=1;i<=nBins;i++){
           double xlo = lX+dx*(i-1);
           double xhi = xlo+dx;
           double xmid = xlo+dx/2.0;
           
            if(xlo>(gams[0]+3.62)){
                itg[0] = (fgs(xlo)+fgs(xmid)+fgs(xhi))*dx/3.0;
                if(xlo>(gams[1]+3.62)){
                    itg[1] = (f3(xlo)+f3(xmid)+f3(xhi))*dx/3.0;
                    if(xlo>(gams[2]+3.62)){
                        itg[2] = (f9(xlo)+f9(xmid)+f9(xhi))*dx/3.0;
                        if(xlo>(gams[3]+3.62))
                            itg[3] = (f5(xlo)+f5(xmid)+f5(xhi))*dx/3.0;
                    }
                }
            }
            outF << std::setprecision(4) << hrel[f][0]->GetBinCenter(i); 
            t_itg = itg[0]+itg[1]+itg[2];
            for (int k=0;k<3;k++){ 
                hrel[f][k]->SetBinContent(hrel[f][k]->FindBin(-xmid),itg[k]/t_itg); habs[f][k]->SetBinContent(habs[f][k]->FindBin(-xmid),itg[k]);
                if(itg[k]>0.0) outF << std::setprecision(8) << "\t" << itg[k];
                else outF << std::setprecision(8) << "\t" << 0.00000001;
                itg[k]=0.0;
            }
            outF << endl;
        }
            outF.close();
    }

    ff[0] = new TF1("f0",fgs,4,10,0); ff[0]->SetLineColor(lc[0]); ff[0]->SetNpx(5000);
    ff[1] = new TF1("f1",f3,4,10,0);  ff[1]->SetLineColor(lc[1]); ff[1]->SetNpx(5000);
    ff[2] = new TF1("f2",f9,4,10,0);  ff[2]->SetLineColor(lc[2]); ff[2]->SetNpx(5000);

    if (kVerbose) cout << "Stacking Histograms..." << endl;

    TCanvas *c1 = new TCanvas();

    if(cType==1){

        //TCanvas *c1 = new TCanvas();
        if(nFiles<3) c1->Divide(1,nFiles);
        else c1->Divide(2,ceil(nFiles/2.));

        for(int i=0;i<nFiles;i++){
            
            c1->cd(i+1);
            c1->GetPad(i+1)->SetFrameLineColor(kWhite);
            hs[i]->Add(hrel[i][0]); hs[i]->Add(hrel[i][1]); hs[i]->Add(hrel[i][2]);


            hs[i]->Draw();
            hs[i]->GetXaxis()->SetLabelSize(0.06);
            hs[i]->GetXaxis()->SetLabelOffset(0.04);
            hs[i]->GetXaxis()->SetNdivisions(508);
            hs[i]->GetYaxis()->SetLabelSize(0.06);

            gGS->DrawClone("same P");
            g1561->DrawClone("same P");
            g854->DrawClone("same P");

        }

        c1->SaveAs("output/canvases/Doorway_Rel.root");


        /* for(int i=0;i<nFiles;i++){
            c1->cd(i+1);
            c1->GetPad(i+1)->SetLogy();
            c1->GetPad(i+1)->SetFrameLineColor(kWhite);
            ha[i]->Add(habs[i][0]); ha[i]->Add(habs[i][2]); ha[i]->Add(habs[i][1]);

            ha[i]->Draw();
            ha[i]->GetXaxis()->SetLabelSize(0.12);
            ha[i]->GetYaxis()->SetLabelSize(0.1);
        }
        c1->SaveAs("output/canvases/Doorway_Abs.root"); */
    }
    else if (cType==2){

        //TCanvas *c1 = new TCanvas();
        c1->Divide(2,1);
        
        for(int i=0;i<nFiles;i++){
            
            c1->cd(1);
            c1->GetPad(1)->SetFrameLineColor(kWhite);
            hs[i]->Add(hrel[i][0]); hs[i]->Add(hrel[i][1]); hs[i]->Add(hrel[i][2]);

            hs[i]->Draw();
            hs[i]->GetXaxis()->SetLabelSize(0.06);
            hs[i]->GetXaxis()->SetNdivisions(508);
            hs[i]->GetYaxis()->SetLabelSize(0.06);

            gGS->DrawClone("same P");
            g1561->DrawClone("same P");
            g854->DrawClone("same P");

            c1->cd(2);
            c1->GetPad(2)->SetLogy();
            c1->GetPad(2)->SetFrameLineColor(kWhite);
            //ha[i]->Add(habs[i][0]); ha[i]->Add(habs[i][2]); ha[i]->Add(habs[i][1]);

            /* habs[i][0]->SetFillColorAlpha(kBlue,0.5);
            habs[i][1]->SetFillColorAlpha(kGreen+1,0.5);
            habs[i][2]->SetFillColorAlpha(kRed+1,0.5); */

            habs[i][0]->Draw();
            habs[i][1]->Draw("same");
            habs[i][2]->Draw("same");

            //ha[i]->Draw();
            //ha[i]->GetXaxis()->SetLabelSize(0.12);
            //ha[i]->GetYaxis()->SetLabelSize(0.1);
        
            c1->SaveAs(Form("output/canvases/Doorway_%s.root",hs[i]->GetName()));
            //c1->SaveAs(Form("output/images/Doorway_%s.png",hs[i]->GetName()));
        }
    }
    
    TFile *fOut = new TFile("output/hFile.root","RECREATE");
    c1->Write();
    for (int i=0;i<nFiles;i++)
        for (int k=0;k<3;k++){
            hrel[i][k]->Write();
            habs[i][k]->Write();
            if(i==0) ff[k]->Write();
        }
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
  handler.add(optionExt("delta",required_argument, NULL,'D',"","Set Histogram Bin Width in MeV (Must be able to factor 6 MeV)"));
  handler.add(optionExt("err",optional_argument, NULL,'E',"","Calculate Chi2 Fit Error"));
  handler.add(optionExt("thresh",required_argument, NULL,'T',"","Set Threshold for Spectroscopic Factors"));
  handler.add(optionExt("canvas",required_argument, NULL,'c',"","Differentiate between canvas styles for histograms.\n                                          (1) Relative and Absolute stacked histograms on separate canvases.\n                                          (2) Relative and Absolute hists on same canvas for each level"));

  
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
    cout << "Setting SF threshold to " << thresh << endl;
  }

  if(handler.getOption(4)->active){
    cType = stoi(handler.getOption(4)->argument);
    //gStyle->SetCanvasPreferGL(true);
    cout << "Setting Canvas Style " << cType << endl;
  }

  const char* filename = argv[argc-1];
  doorway(filename);
   
 return 0;
}
