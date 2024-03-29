#include <iostream>
#include <map>

#include "HFReader.hpp"
#include "FileReader.hpp"
#include "VandleEff.hpp"
#include "dummyFuncs.hpp"
#include "optionHandler.hpp"

#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
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
bool kErr;
bool kZoom;
bool kPrintRel;

double dx = 0.05;
int maxS=-1;
int minS=-1;

double perror( vector<pair<double,pair<double,double>>> pIn);
double chi2Err( vector<pair<double,pair<double,double>>> pIn);

void summarize(const char *filename_){
    //gStyle->SetEndErrorSize(8.5);
    gStyle->SetOptStat(0);
    
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

    gGS->SetMarkerStyle(8); g1561->SetMarkerStyle(8); g854->SetMarkerStyle(8);
    gGS->SetLineWidth(2); g1561->SetLineWidth(2); g854->SetLineWidth(2);
    gGS->SetLineColor(kBlue); g1561->SetLineColor(kRed+2); g854->SetLineColor(kGreen+3);
    gGS->SetMarkerColor(kBlue); g1561->SetMarkerColor(kRed+2); g854->SetMarkerColor(kGreen+3);

    for (int i=0;i<nrow;i++){
        double Ex = stod(bgtS.at(i).at(0));
        double bGS = stod(bgtS.at(i).at(1));
        double b1561 = stod(bgtS.at(i).at(3));
        double b854 = stod(bgtS.at(i).at(5));
        double bAll = bGS+b1561+b854;

        double eGS = stod(bgtS.at(i).at(2));
        double e1561 = stod(bgtS.at(i).at(4));
        double e854 = stod(bgtS.at(i).at(6));

        double elo=0, ehi=0;
        double bnew;

        if(bGS>0.0)   {
            gGS->SetPoint(i,Ex,bGS/bAll); 
            elo = bGS/bAll-(bGS-eGS)/(bAll-eGS);
            ehi = (bGS+eGS)/(bAll+eGS)-bGS/bAll;
            gGS->SetPointError(i,0.0,0.0,elo,ehi);}
        if(b1561>0.0) {
            bnew = bGS+b1561;
            g1561->SetPoint(i,Ex,bnew/bAll); 
            elo = bnew/bAll-(bnew-e1561)/(bAll-e1561);
            ehi = (bnew+e1561)/(bAll+e1561)-bnew/bAll;
            g1561->SetPointError(i,0.0,0.0,elo,ehi);}
        if(b854>0.0)  {
            bnew = bGS+b1561+b854;
            g854->SetPoint(i,Ex,bnew/bAll); 
            elo = bnew/bAll-(bnew-e854)/(bAll);
            ehi = (bnew+e854)/(bAll+e854)-bnew/bAll;
            g854->SetPointError(i,0.0,0.0,elo,0.0);}

        if(kPrintRel)
            printf("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",Ex,bGS/bAll,eGS/bAll,(bGS+b1561)/bAll,e1561/bAll,(bGS+b1561+b854)/bAll,e854/bAll);

        for(int j=0;j<ncol;j++) dummy.push_back(stod(bgtS.at(i).at(j)));
        dummy.push_back(bAll);
        bgt.push_back(dummy);
        dummy.clear();
        
    }

    
    FileReader fl(filename_);
    fl.SetVerbose(kVerbose);
    vector< vector< string > > fList = fl.LoadFile();
    map<double,vector<vector<double>>> beoh;
    //Loop over files summary files here.
    unsigned int nFiles = fList.size();
    fl.CloseFile();
    int nSpins;

    FileReader fr;
    fr.SetVerbose(kVerbose);
    vector<vector<double>> dummy2;
    vector<double> states;

    for (int f=0;f<nFiles;f++){
    if(fr.IsOpen()) fr.Reset();
    
    fr.SetFileName(fList.at(f).at(0).c_str());
    fr.OpenFile();
    vector<vector<string>> finfo = fr.LoadFile();
        
    int nRows = finfo.size();
    nSpins = nRows;
    if (nRows==0) continue;
    int nCols = finfo.at(0).size();
    if (nCols==0) continue;
    double Ex = stod(finfo.at(0).at(0));
    states.push_back(Ex);

        for (int i=0;i<nRows;i++){

            double jj = stod(finfo.at(i).at(1));

            double bhgs = stod(finfo.at(i).at(4));
            double bh854 = stod(finfo.at(i).at(5));
            double bh1561 = stod(finfo.at(i).at(6));
            double bh2004 = stod(finfo.at(i).at(7));

            dummy.push_back(jj); dummy.push_back(bhgs); dummy.push_back(bh1561); dummy.push_back(bh854); 
            dummy.push_back(bh2004);
            //dummy.push_back(stod(finfo.at(i).at(2)));
            dummy.push_back(bhgs + bh854 + bh1561);
            
            dummy2.push_back(dummy);
            dummy.clear();
        }
        beoh.insert(make_pair(Ex,dummy2)); 
        dummy2.clear();
    }
    cout << "Done Loading Files" << endl;
    fr.Reset();
    if (beoh.size()!=nFiles) {
        cout << "[HFSummary] Failure in Summary File loading\nnSpins=" << nFiles << ", beoh=" << beoh.size() << endl;
        return;
    }

    if(kVerbose) cout << "[HFSummary] From BEOH: nSpins=" << nSpins << ", nStates=" << beoh.size() << endl;
    //Make bin array for histograms
    const int nXbins = states.size();
    double xbinL[nXbins+1];
    double val;
    for (int i=0;i<nXbins-1;i++){
        val = 0.5*(states.at(i)+states.at(i+1));
        if(i==0) xbinL[0] = states.at(0)-(val-states.at(0));
        xbinL[i+1]=val;
    }
    xbinL[nXbins] = states.at(nXbins-1)+(val-states.at(nXbins-1));
    
    //Fill histograms
    if(kVerbose) cout << "Filling Spin Histograms" << endl;
    
    if(minS<0&&maxS<0){minS=0;maxS=nSpins-1;}
    else if(minS>0&&maxS<0){maxS=nSpins-1;}
    else if(maxS>0&&minS<0){minS=0;}
    //else if(minS>0&&maxS>0){nSpins = maxS-minS+1;}
    
    if (minS<0||maxS>9) {cout << "Error in setting max and min J" << endl; return;}

    TH1F *hCalc[3][nSpins];
    int lc[3] = {kCyan-9,kRed-9,kGreen-9};
    for(int i=0; i<3; i++){ 
        for (int h=0;h<nSpins;h++){
            hCalc[i][h] = new TH1F(Form("h%d_J%d",i,h),Form("h%d_J%d",i,h),nXbins,xbinL);
            hCalc[i][h]->SetFillColor(lc[i]); 
            hCalc[i][h]->SetLineColor(lc[i]); 
        }
    }

    THStack *hs[nSpins];
        for(int i=0;i<nSpins;i++) hs[i] = new THStack(Form("hJ%d",i),Form("J %d-",i));

    
    vector<vector<double>> evec;
    for(map<double,vector<vector<double>>>::iterator it=beoh.begin();it!=beoh.end();it++){
        double ex = (*it).first;
        evec = (*it).second;
        int binID = hCalc[0][0]->FindBin(ex);
        for(int i=0;i<nSpins;i++){
            for(int s=0;s<3;s++){
            hCalc[s][i]->SetBinContent(binID,evec.at(i).at(s+1)/evec.at(i).at(5));
            }
        }
    }
    
    if (kVerbose) cout << "Stacking Histograms..." << endl;

    TCanvas *c1 = new TCanvas();
    int ds = maxS-minS+1;
    c1->Divide(2,ceil(ds/2.));
    for(int i=minS;i<maxS+1;i++){
        int ci=i-minS;
        c1->cd(ci+1);
        c1->GetPad(ci+1)->SetFrameLineColor(kWhite);
        hs[i]->Add(hCalc[0][i]); hs[i]->Add(hCalc[1][i]); hs[i]->Add(hCalc[2][i]);
        
        hs[i]->Draw();
        hs[i]->GetXaxis()->SetLabelSize(0.12);
        if(kZoom) hs[i]->GetXaxis()->SetRangeUser(6.0,8.5);
        hs[i]->GetYaxis()->SetLabelSize(0.1);
        
        gGS->DrawClone("same P");
        g1561->DrawClone("same P");
        g854->DrawClone("same P");
    }

    c1->SaveAs("output/canvases/HFSummary_c1.root");
    
    //double chi[nSpins];

    vector<pair<double,pair<double,double>>> GTPair;
    vector<pair<double,pair<double,double>>> FFPair;
    double xi2GT, xi2FF;

    //TFile *fOut = new TFile("SummaryHists.root","RECREATE");
    FILE *outF = fopen("output/TSV/chi2Vals.tsv","w");
    fprintf(outF,"Spin\tE\tGT\tFF\n");
    
    for(int j=minS; j<maxS+1;j++){
        
        if(kVerbose) cout << "Calculating Chi2 for Spin J=" << j << endl;
        
        for(int i=0; i<bgt.size(); i++){
            double Ex=bgt.at(i).at(0);
            double bAll=bgt.at(i).at(7);
            int binID=hCalc[0][j]->FindBin(Ex);

            double sum1=0.0,sum2=0.0;

            GTPair.clear();
            FFPair.clear();
            xi2GT=0.0; xi2FF=0.0;
            if(kPrintRel) printf("%d %.2f",j,Ex);
            for(int k=0;k<3;k++){
                if(Ex>6.8&&Ex<9){
                 if(bgt.at(i).at(2*k+1)>0.0)
                  GTPair.push_back(make_pair(hCalc[k][j]->GetBinContent(binID)*bAll,make_pair(bgt.at(i).at(2*k+1),bgt.at(i).at(2*k+2))));
                  //GTPair.push_back(make_pair(hCalc[k][j]->GetBinContent(binID),make_pair(bgt.at(i).at(2*k+1)/bAll,bgt.at(i).at(2*k+2)/bgt.at(i).at(2*k+1))));
                  sum1+=hCalc[k][j]->GetBinContent(binID);
                  sum2+=bgt.at(i).at(2*k+1)/bAll;
                  
                  if(kPrintRel)
                    printf("\t%.2f",sum1-sum2);
                }
                else{
                 if(bgt.at(i).at(2*k+1)>0.0)
                  FFPair.push_back(make_pair(hCalc[k][j]->GetBinContent(binID)*bAll,make_pair(bgt.at(i).at(2*k+1),bgt.at(i).at(2*k+2))));
                }
            }
            if(kPrintRel) printf("\n");
            if(kVerbose&&GTPair.size()>0){
                cout << "Ex " << Ex << " GT Size: " << GTPair.size() << endl;
                for (int k=0;k<GTPair.size();k++) cout<<GTPair.at(k).first<<" "<<GTPair.at(k).second.first<<" "<<GTPair.at(k).second.second<<endl;
            }
            
            if(kErr){
                if(GTPair.size()>0) xi2GT = chi2Err(GTPair);
                if(FFPair.size()>0) xi2FF = chi2Err(FFPair);
            }
            else{
                if(GTPair.size()>0) xi2GT = perror(GTPair);
                if(FFPair.size()>0) xi2FF = perror(FFPair);
            }
            
            fprintf(outF,"Ex:\t%f\tJ%d\t%.2f\t%.2f\n", Ex, j, xi2GT,xi2FF);
            //if(kVerbose) printf("Ex:\t%f\tGT X^2/Ndf: %.2f, FF X^2/Ndf: %.2f\n", Ex, xi2GT, xi2FF);
        }
        fprintf(outF,"\n");
    }
    
    fclose(outF);
    return;
}

double perror(vector<pair<double,pair<double,double>>> pIn){
    double retval=0;
    double sum=0;
    int ni=0;
    for(int i=0;i<pIn.size();i++){
        sum += pIn.at(i).second.first;
    }
    for(int i=0;i<pIn.size();i++){
        if(pIn.at(i).second.first==0||pIn.at(i).first==0) retval += 0.0;
        else {retval += abs(pIn.at(i).first-pIn.at(i).second.first)/abs(pIn.at(i).first+pIn.at(i).second.first); ni++;}
        //else {retval += abs(pIn.at(i).first-pIn.at(i).second.first)/abs(pIn.at(i).first+pIn.at(i).second.first)*pIn.at(i).second.first/sum; ni++;}
    }
    return retval/ni;

};

double chi2Err(vector<pair<double,pair<double,double>>> pIn){
    double retval=0;
    int ni=0;
    double sum=0;
    for(int i=0;i<pIn.size();i++){
        sum += pIn.at(i).second.first;
    }
    for(int i=0;i<pIn.size();i++){
        //printf("((%.5f-%.5f)/%.5f)^2 + ",pIn.at(i).first,pIn.at(i).second.first,pIn.at(i).second.second);
        if(pIn.at(i).second.first==0||pIn.at(i).first==0) retval += 0.0;
        else {retval += pow(abs(pIn.at(i).first-pIn.at(i).second.first)/abs(pIn.at(i).second.second),2); ni++;}
        //printf("%.4f => ",retval);
    }
    //printf(" %.3f\n",retval/ni);
    return retval/ni;
}


int main(int argc, char **argv){
    
  gROOT->SetBatch(true);

  optionHandler handler;
  handler.add(optionExt("debug",optional_argument, NULL,'d',"","Turn on verbose output"));
  handler.add(optionExt("delta",required_argument, NULL,'D',"","Set Histogram Bin Width in MeV (Must be able to factor 6 MeV)"));
  handler.add(optionExt("err",optional_argument, NULL,'E',"","Calculate Chi2 Fit Error"));
  handler.add(optionExt("zoom",optional_argument, NULL,'z',"","Zoom X axis around GT transitions for canvases"));
  handler.add(optionExt("minS",required_argument, NULL,'s',"","Minimum spin to use from HF calculations"));
  handler.add(optionExt("maxS",required_argument, NULL,'S',"","Max spin to use from HF Calculations"));
  handler.add(optionExt("rel",optional_argument, NULL,'r',"","Print Relative Intensities from data"));
  
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
    cout << "Energy Axis GT zoom enables" << endl;
    kZoom = true;
  }else
    kZoom = false;

  if(handler.getOption(4)->active){
    minS = stoi(handler.getOption(4)->argument);
    cout << "Setting Min spin to " << minS << endl;
  }
  if(handler.getOption(5)->active){
    maxS = stoi(handler.getOption(5)->argument);
    cout << "Setting Max spin to " << maxS << endl;
  }

  if(handler.getOption(6)->active){
    cout << "Printing Relative Branching Ratios from data" << endl;
    kPrintRel = true;
  }else
    kPrintRel = false;

  const char* filename = argv[argc-1];
  summarize(filename);
   
 return 0;
}


                    /* if(bgt.at(i).at(1)>0.0)
                        GTPair.push_back(make_pair(hCalc[0][j]->GetBinContent(binID),make_pair(bgt.at(i).at(1)/bAll,bgt.at(i).at(2)/bgt.at(i).at(1))));
                    if(bgt.at(i).at(3)>0.0)
                        GTPair.push_back(make_pair(hCalc[1][j]->GetBinContent(binID),make_pair(bgt.at(i).at(3)/bAll,bgt.at(i).at(4)/bgt.at(i).at(3))));
                    if(bgt.at(i).at(5)>0.0)
                        GTPair.push_back(make_pair(hCalc[2][j]->GetBinContent(binID),make_pair(bgt.at(i).at(5)/bAll,bgt.at(i).at(2)/bgt.at(i).at(1)))); */

                                            /* if(bgt.at(i).at(1)>0.0)
                        FFPair.push_back(make_pair(hCalc[0][j]->GetBinContent(binID),make_pair(bgt.at(i).at(1)/bAll,bgt.at(i).at(2)/bgt.at(i).at(1))));
                    if(bgt.at(i).at(3)>0.0)
                        FFPair.push_back(make_pair(hCalc[1][j]->GetBinContent(binID),make_pair(bgt.at(i).at(3)/bAll,bgt.at(i).at(2)/bgt.at(i).at(1))));
                    if(bgt.at(i).at(5)>0.0)
                        FFPair.push_back(make_pair(hCalc[2][j]->GetBinContent(binID),make_pair(bgt.at(i).at(5)/bAll,bgt.at(i).at(2)/bgt.at(i).at(1)))); */
