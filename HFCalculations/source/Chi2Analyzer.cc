#include <iostream>
#include <map>

#include "HFReader.hpp"
#include "FileReader.hpp"
#include "VandleEff.hpp"
#include "dummyFuncs.hpp"

#include "TGraphErrors.h"
#include "TH2F.h"
#include "TH1I.h"
#include "TFile.h"

using namespace std;

double chi2Calc( vector<pair<double,double>> pIn);

void chi2Analyzer(const char *filename_, bool kVerbose=false){

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


    for (int i=0;i<nrow;i++){
        for(int j=0;j<ncol;j++) dummy.push_back(stod(bgtS.at(i).at(j)));
        bgt.push_back(dummy);
        dummy.clear();
    }

    //string filename_ = "sm_gtjoe.summary";
    //FileReader fr(filename_.c_str());
    FileReader fr(filename_);
    fr.SetVerbose(kVerbose);
    vector<vector<string>> finfo = fr.LoadFile();
    
    std::map<double,vector<vector<double>>> beoh;
    vector<vector<double>> dummy2;
    int nRows = finfo.size();
    int nCols = finfo.at(0).size();
    double oldEx, newEx;

    for (int i=0;i<nRows;i++){
        newEx = stod(finfo.at(i).at(0));
        if(oldEx!=newEx){ 
            beoh.insert(std::make_pair(oldEx,dummy2)); 
            dummy2.clear();            
            oldEx=newEx;
        }
        double bhgs = stod(finfo.at(i).at(4));
        double bh1561 = stod(finfo.at(i).at(6));
        double bh854 = stod(finfo.at(i).at(5));
        
        dummy.push_back(bhgs); dummy.push_back(bh1561); dummy.push_back(bh854);
        dummy.push_back(bhgs+bh1561+bh854);
        dummy2.push_back(dummy);
        dummy.clear();
    }
    beoh.insert(std::make_pair(oldEx,dummy2)); 
    dummy2.clear();

    double xlo=4;
    double xhi=10;
    double width=0.05;
    int nBins = (xhi-xlo)/width;
    
    TH2F *hChi = new TH2F("hChi","hChi",11,-0.5,10.5,nBins,xlo,xhi);
    TH2F *nChi = new TH2F("nChi","nChi",11,-0.5,10.5,nrow,0,nrow);
    TH1F *hxi[nrow];
    const char *levels[27] = {"9.80", "9.23", "8.79", "8.54", "8.19", "7.75", "7.37", "7.18", "6.88", "6.76", "6.58", "6.37", "6.23", "6.09", "5.99", "5.89", "5.60", "5.13", "5.04", "4.96", "4.84", "4.70", "4.53", "4.31", "4.21", "4.09", "4.04"};
    double chi[9];
    double chisum;

    vector<pair<double,double>> Apair;
    double xi2;

    TFile *fOut = new TFile("chi2Hists.root","RECREATE");
    FILE *outF = fopen("chi2Vals.tsv","w");

    for(int i=0; i<nrow; i++){
        //Data Points
        double Ex = bgt.at(i).at(0);
        double gsA = bgt.at(i).at(1);
        double gam1A = bgt.at(i).at(3);
        double gam2A = bgt.at(i).at(5);
        double totA = gsA+gam1A+gam2A;

        //BEOH Calculations
        if(beoh.count(Ex)==1){
            hxi[i] = new TH1F(Form("h_%.0f",Ex*1000),Form("%.0f",Ex*1000),11,-0.5,10.5);
            cout << "Found State Info for BGT Level " << Ex << endl;
            dummy2 = beoh[Ex];
            chisum = 0;

            if(kVerbose){
            for(int ii=0;ii<dummy2.size();ii++){
                cout << ii << " " << dummy2.at(ii).size() << "\t" ;
                for(int jj=0;jj<dummy2.at(ii).size();jj++){
                    cout << dummy2.at(ii).at(jj) << " " ; 
                }
                cout << endl;
            }
            }
            for(int j=0; j<9;j++){
                Apair.clear();
                double Atot = dummy2.at(j).at(3);
                if(Atot!=0.0){
                double scale = totA/Atot;
                Apair.push_back(make_pair(gsA,dummy2.at(j).at(0)*scale));
                Apair.push_back(make_pair(gam1A,dummy2.at(j).at(1)*scale));
                Apair.push_back(make_pair(gam2A,dummy2.at(j).at(2)*scale));
                
                //for (int k=0;k<Apair.size();k++) cout << Apair.at(k).first << " " << Apair.at(k).second << "\t";
                
                xi2 = chi2Calc(Apair);
                chi[j] = xi2;
                chisum += xi2;
                //cout << xi2 << endl;
                } else xi2 = 0.0;
                chi[j] = xi2;
                chisum += xi2;
                int xbin = hChi->GetXaxis()->FindBin(j+1);
                int ybin = hChi->GetYaxis()->FindBin(Ex);
                if(xi2>0.0){
                    hChi->SetBinContent(xbin,ybin,xi2);
                    hxi[i]->SetBinContent(xbin,xi2);
                }
            }
            fprintf(outF,"%.2f",Ex);
            for(int k=0;k<9;k++){
                nChi->SetBinContent(k+2,i,chi[k]/chisum);
                fprintf(outF,"\t%.3f",chi[k]/chisum);
            }
            fprintf(outF,"\n");
            hxi[i]->Write();
        }
    }

    for (int l=1;l<=nrow;l++) nChi->GetYaxis()->SetBinLabel(l,levels[l-1]);
    
    fOut->Close();
    hChi->SaveAs("beoh_chi2.root");
    nChi->SaveAs("beoh_normChi2.root");

    fclose(outF);
    return;
}

double chi2Calc(vector<pair<double,double>> pIn){
    double retval=0;
    int ni=0;
    for(int i=0;i<pIn.size();i++){
        if(pIn.at(i).second==0||pIn.at(i).first==0) retval += 0.0;
        else {retval += pow(pIn.at(i).first-pIn.at(i).second,2)/abs(pIn.at(i).first); ni++;}
    }
    return retval/ni;
};

int main(int argc, char **argv){
    
    if(argc<2||argc>3){ cout << "Usage: chi2Analyzer [FileName] <Optional Verbose Boolean(true/false)>" << endl; return 1;}
    else{
        const char* filename = argv[1];
        if(argc==3){
            chi2Analyzer(filename,!(strcmp(argv[2],"true")));
        }else chi2Analyzer(filename);
    }

    return 0;
}