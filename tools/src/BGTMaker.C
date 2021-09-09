#include "FileReader.C"

using namespace std;

int lc[3] = {kCyan+3,kCyan-7,kCyan-3};
//int lc[3] = {kCyan-9,kRed-9,kGreen-9};
//int ld[3] = {kBlue, kRed+2, kGreen+3};
int ld[3] = {kRed, kRed, kRed};

double FermiIntegral(int Zed, double Emax){//integration of Fermi function, energy in keV
   double logf=0;
   double evalCoeff[4]={0};
   double coeff [16] ={-17.2,7.9015,-2.54,0.28482,3.31368,-2.06273,0.703822,-0.075039,-0.364018,0.387961,-0.142528,0.016,0.0278071,-0.026519,0.0098854,-0.00113772};
   evalCoeff[0]=coeff[0]+coeff[1]*log(Zed)+coeff[2]*pow(log(Zed),2)+coeff[3]*pow(log(Zed),3);
   evalCoeff[1]=coeff[4]+coeff[5]*log(Zed)+coeff[6]*pow(log(Zed),2)+coeff[7]*pow(log(Zed),3);
   evalCoeff[2]=coeff[8]+coeff[9]*log(Zed)+coeff[10]*pow(log(Zed),2)+coeff[11]*pow(log(Zed),3);
   evalCoeff[3]=coeff[12]+coeff[13]*log(Zed)+coeff[14]*pow(log(Zed),2)+coeff[15]*pow(log(Zed),3);
   logf = evalCoeff[0]+evalCoeff[1]*log(Emax)+evalCoeff[2]*pow(log(Emax),2)+evalCoeff[3]*pow(log(Emax),3);
   return logf;
}

double calcTOF(double E){
    return 100.0/29.98*sqrt(938.9/(2*E));
}

double calcErr(double E){
    double tof = calcTOF(E);
    return sqrt(pow(2*1.0/tof,2)+pow(2*0.03/1,2))*E;
}

void BGTMaker(const char *filename_=NULL, bool kPrint=false){
    if(filename_==NULL) { cout << "No File Name Specified" << endl; return;}
    gStyle->SetOptStat(0);
    double xlo=0;
    double xhi=15;
    double width=0.15;
    int nBins = (xhi-xlo)/width;
    TH1F *h1561 = new TH1F("h1561","",nBins,xlo,xhi);
    TH1F *h854 = new TH1F("h854","",nBins,xlo,xhi);
    TH1F *hAll = new TH1F("hAll","",nBins,xlo,xhi);

    FileReader fr(filename_);

    vector<vector<string>> lInfo = fr.LoadFile();
    int nlines = lInfo.size();
    for (int i=0;i<nlines;i++){
        int ncol = lInfo.at(i).size();
        double Ex = stod(lInfo.at(i).at(0));
        double an = stod(lInfo.at(i).at(ncol-1));

        int binID = hAll->FindBin(Ex);
        hAll->SetBinContent(binID,hAll->GetBinContent(binID)+an);
        if(ncol==4){
            if(lInfo.at(i).at(1)=="1561") h1561->SetBinContent(binID,an);
            if(lInfo.at(i).at(1)=="854") h854->SetBinContent(binID,an);
        }
    }

    hAll->SetBarWidth(1);
    h1561->SetBarWidth(0.5);
    h1561->SetBarOffset(0.5);
    h854->SetBarWidth(0.5);
    h854->SetBarOffset(0);


    hAll->SetFillColor(kBlack);
    h1561->SetFillColor(kCyan);
    h854->SetFillColor(kMagenta);

    int hMax = ceil(hAll->GetMaximum());
    hAll->GetXaxis()->SetRangeUser(3.62,10);
    hAll->GetYaxis()->SetRangeUser(1,hMax*1.1);

    hAll->Draw("hbar");
    h1561->Draw("hbar same");
    h854->Draw("hbar same");

    if(kPrint){
        for (int ib=1;ib<=nBins;ib++)
            if(hAll->GetBinContent(ib)>0.01) cout << std::fixed << std::setprecision(2) 
                                               << hAll->GetBinCenter(ib) << "\t" 
                                               << hAll->GetBinContent(ib)-(h1561->GetBinContent(ib)+h854->GetBinContent(ib)) << "\t" 
                                               << h1561->GetBinContent(ib) << "\t" 
                                               << h854->GetBinContent(ib) << "\n";
    }

    return;
}

void HistMaker(const char *filename_=NULL, bool kPrint=false){
    if(filename_==NULL) { cout << "No File Name Specified" << endl; return;}
    gStyle->SetOptStat(0);
    double xlo=0;
    double xhi=15;
    double width=0.25;
    int nBins = (xhi-xlo)/width;
    TH1F *h1561 = new TH1F("h1561","",nBins,xlo,xhi);
    TH1F *h854 = new TH1F("h854","",nBins,xlo,xhi);
    TH1F *hGS = new TH1F("hGS","",nBins,xlo,xhi);

    THStack *hs = new THStack("hs","Stacked BGT");
    FileReader fr(filename_);

    vector<vector<string>> lInfo = fr.LoadFile();
    int nlines = lInfo.size();
    
    for (int i=0;i<nlines;i++){
        int ncol = lInfo.at(i).size();
        double Ex = stod(lInfo.at(i).at(0));
        int binID = hGS->FindBin(Ex);
        hGS->SetBinContent(binID,stod(lInfo.at(i).at(1)));
        h1561->SetBinContent(binID,stod(lInfo.at(i).at(2)));
        h854->SetBinContent(binID,stod(lInfo.at(i).at(3)));
    }

    hGS->SetFillColor(kBlack);
    h1561->SetFillColor(kBlue);
    h854->SetFillColor(kRed);

    hGS->SetLineColor(kBlack);
    h1561->SetLineColor(kBlue);
    h854->SetLineColor(kRed);

    hs->Add(hGS);
    hs->Add(h1561);
    hs->Add(h854);
    hs->Draw();

    TLegend *l1 = new TLegend(0.3,0.3,"Feeding");
    l1->AddEntry(hGS,"7/2(GS)","f");
    l1->AddEntry(h1561,"9/2 (1561)","f");
    l1->AddEntry(h854,"3/2 (854)","f");
    //l1->Draw("same");

    if(kPrint){
        for (int ib=1;ib<=nBins;ib++)
            if(hGS->GetBinContent(ib)>0.01) cout << std::fixed << std::setprecision(2) 
                                               << hGS->GetBinCenter(ib) << "\t" 
                                               << hGS->GetBinContent(ib) << "\t" 
                                               << h1561->GetBinContent(ib) << "\t" 
                                               << h854->GetBinContent(ib) << "\n";
    }

    return;
}

void OldGraphErrMaker(const char *filename_=NULL, bool kPrint=false, bool kBar=false, bool kShade=false){

    gROOT->Reset();
    gStyle->SetBarWidth(0.4);
    
    int hScale = 1;
    
    if(kShade) gROOT->SetBatch(true);
    double icount=0, Pn=0.60;
    double t = 0.140;

    if(filename_==NULL) { cout << "No File Name Specified" << endl; return;}
    gStyle->SetOptStat(0);
    //gStyle->SetEndErrorSize(7);
  
    TGraphErrors *g1561 = new TGraphErrors(); g1561->SetName("g1561");
    TGraphErrors *g854 = new TGraphErrors(); g854->SetName("g854");
    TGraphErrors *gGS = new TGraphErrors(); gGS->SetName("gGS");

    TMultiGraph *gall = new TMultiGraph("gall","gall");

    FileReader fr(filename_);

    double elow=6.0, ehi=8.5;

    vector<vector<string>> lInfo = fr.LoadFile();
    int nlines = lInfo.size();

    vector <double> itot;
    vector <double> itoterr;

    double tempE = 0;
    int ip=0;

    for (int i=0;i<nlines;i++){
        int ncol = lInfo.at(i).size();
        double Ex = stod(lInfo.at(i).at(0));
        double gsA = stod(lInfo.at(i).at(1));
        double gam9 = stod(lInfo.at(i).at(3));
        double gam3 = stod(lInfo.at(i).at(5));
        
        double gsE = stod(lInfo.at(i).at(2));
        double gam9E = stod(lInfo.at(i).at(4));
        double gam3E = stod(lInfo.at(i).at(6));
        
        if(Ex>elow&&Ex<ehi){
            
            if(gsA>0.01){
                tempE = stod(lInfo.at(i).at(2))/stod(lInfo.at(i).at(1));
                gGS->SetPoint(ip,-Ex,gsA); //gGS->SetPointError(ip,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gsA);
                //gGS->SetPointError(ip,0.0,stod(lInfo.at(i).at(2)));
                gGS->SetPointError(ip,calcErr(Ex-3.62-1.561)/2.,gsE);
            //}
            //if(gam9>0.01){
                tempE = stod(lInfo.at(i).at(4))/stod(lInfo.at(i).at(3));
                g1561->SetPoint(ip,-Ex,gsA+gam3+gam9); //g1561->SetPointError(ip,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gam9);
                g1561->SetPointError(ip,calcErr(Ex-3.62-1.561)/2.,gam9E);
            //}
            //if(gam3>0.01){
                tempE = stod(lInfo.at(i).at(6))/stod(lInfo.at(i).at(5));
                g854->SetPoint(ip,-Ex,gsA+gam3); //g854->SetPointError(ip,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gam3);
                g854->SetPointError(ip,calcErr(Ex-3.62-1.561)/2.,gam3E);

                cout << Ex << " " << calcTOF(Ex-3.62-1.561) << " " << calcErr(Ex-3.62-1.561)/2. << endl;
            }
            ip++;
        }

        itot.push_back(gsA+gam9+gam3);
        itoterr.push_back(sqrt(gsE*gsE+gam9E*gam9E+gam3E*gam3E));
        icount += gsA+gam9+gam3;
        //lf.push_back(FermiIntegral(50,(14.6-Ex)*1000));
        //lferr.push_back(0);
    }

    icount = icount/Pn;
    
    if(kPrint){
    for (int i=0;i<itot.size();i++){
        double Ex = stod(lInfo.at(i).at(0));
        double tof = calcTOF(Ex-3.62);
        double Eerr = sqrt(pow(1.2/tof,2)+ pow(3/100.,2))*Ex;
        
        double lf = FermiIntegral(50,(14.6-Ex)*1000.);
        double lf_hi = FermiIntegral(50,(14.6-(Ex+Eerr))*1000.);
        double lf_lo = FermiIntegral(50,(14.6-(Ex-Eerr))*1000.);
        
        double th = t/(itot.at(i)/icount);
        double th_hi = t/((itot.at(i)+itoterr.at(i))/icount);
        double th_lo = t/((itot.at(i)-itoterr.at(i))/icount);
        
        
        double bgt = 6144./(pow(10,lf)*th);
        double bgt_hi = 6144./(pow(10,lf_hi*th_hi));
        double bgt_lo = 6144./(pow(10,lf_lo*th_lo));
        
        printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf_hi+log10(th_hi), bgt_hi, lf_lo+log10(th_lo), bgt_lo);
        //printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf_hi+log10(th), bgt_hi, lf_lo+log10(th), bgt_lo);
        //printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf+log10(th_hi), bgt_hi, lf+log10(th_lo), bgt_lo);
    }
    }

    TCanvas *c1 = new TCanvas("c1","c1",900,600);
    TPad *p1,*p2, *p3;
    if (!kBar){
        p1 = new TPad("p1","p1",0,0,1,0.5);
        p2 = new TPad("p2","p2",0,0.5,1,0.75);
        p3 = new TPad("p3","p3",0,0.75,1,1);
    } else{
        p1 = new TPad("p1","p1",0,0,0.5,1);
        p2 = new TPad("p2","p2",0.5,0,0.75,1);
        p3 = new TPad("p3","p3",0.75,0,1,1);
    }
    
    p1->Draw(); p2->Draw(); p3->Draw();
    
    p1->cd();

    gGS->SetMarkerStyle(1);     g854->SetMarkerStyle(1);       g1561->SetMarkerStyle(1);   
    gGS->SetMarkerSize(0);      g854->SetMarkerSize(0);       g1561->SetMarkerSize(0);   
    gGS->SetLineWidth(0);       g854->SetLineWidth(0);          g1561->SetLineWidth(0);      
    gGS->SetLineColor(ld[0]);   g854->SetLineColor(ld[1]);      g1561->SetLineColor(ld[2]);  
    gGS->SetMarkerColor(lc[0]); g854->SetMarkerColor(lc[1]);    g1561->SetMarkerColor(lc[2]);
    gGS->SetFillColor(lc[0]);   g854->SetFillColor(lc[1]);      g1561->SetFillColor(lc[2]);  
    

    gGS->GetXaxis()->SetLimits(-ehi,-elow); g1561->GetXaxis()->SetLimits(-ehi,-elow); g854->GetXaxis()->SetLimits(-ehi,-elow);

    TGraphErrors *ggGS = (TGraphErrors*)gGS->Clone("ggGS");
    TGraphErrors *gg854 = (TGraphErrors*)g854->Clone("gg854");
    TGraphErrors *gg1561 = (TGraphErrors*)g1561->Clone("gg1561");

    gall->Add(gg1561);
    gall->Add(gg854);
    gall->Add(ggGS);

    gall->GetXaxis()->SetLimits(-ehi,-elow);
    

    gall->GetXaxis()->SetNdivisions(506);
    gall->GetXaxis()->SetLabelOffset(0.015);
    gall->GetXaxis()->SetLabelSize(0.06);
    for(int il=1;il<=6;il++) gall->GetXaxis()->ChangeLabel(-il,90,-1,-1,-1,-1,Form("%.1f",6.0+(il-1)*0.5));
    
    gall->GetYaxis()->SetLimits(0,12000);
    gall->GetYaxis()->SetRangeUser(10,12000);
    gall->GetYaxis()->SetLabelOffset(0.015);
    gall->GetYaxis()->SetNdivisions(503);
    for(int il=1;il<=3;il++) gall->GetYaxis()->ChangeLabel(il,90);
    
    gall->GetYaxis()->SetRangeUser(10,12000);
    gall->Draw("AB");

    gGS->SetLineWidth(2); g854->SetLineWidth(2); g1561->SetLineWidth(2);
    gGS->SetLineColor(kRed+3); g854->SetLineColor(kRed-4); g1561->SetLineColor(kRed+1);

    gGS->SetFillStyle(0); g854->SetFillStyle(0); g1561->SetFillStyle(0);

    g1561->Draw("5 same");
    g854->Draw("5 same"); 
    gGS->Draw("5 same");

    if (kPrint){
        for (int jj=1;jj<gGS->GetN(); jj++) cout << gGS->GetPointX(jj) << "\t" << g854->GetPointX(jj) << "\t" << g1561->GetPointX(jj) << endl;
    }

    TLegend *l1 = new TLegend(0.65,0.6,0.85,0.9,"Feeding");
    l1->AddEntry(ggGS,"7/2(GS)","pf"); l1->AddEntry(gg854,"3/2 (854)","pf"); l1->AddEntry(gg1561,"9/2 (1561)","pf");
    if(kBar) l1->SetTextSize(40);
    //l1->Draw("same");

    //TLegend *lgt[2];
            
    p2->cd();
    TFile *f6GT = TFile::Open("./input/sbeta_GT_6m.root");
    TH1F *h6GT =  (TH1F*)f6GT->Get("sbetaGT_smf1");
    h6GT->SetLineWidth(0);
    
    h6GT->GetXaxis()->SetRangeUser(6.0,8.5);
    h6GT->GetXaxis()->SetTitle("");
    h6GT->GetXaxis()->SetLabelOffset(0.04);
    h6GT->GetXaxis()->SetLabelSize(0.07);
    
    h6GT->GetYaxis()->SetLabelOffset(0.02);
    h6GT->GetYaxis()->SetLabelSize(0.07);
    h6GT->GetYaxis()->SetTickLength(0.02);
    h6GT->GetYaxis()->SetTitleSize(0.07);
    h6GT->GetYaxis()->SetTitleOffset(0.60);
    h6GT->GetYaxis()->SetRangeUser(0.01,1.6);
    


    if(!kBar) h6GT->Draw("hist Y+");
    else h6GT->Draw("hbar");
    //lgt[0] = new TLegend(0.5,0.65,0.8,0.8);
	//lgt[0]->AddEntry(h6GT,"^{134}In J^{#pi}: 6^{-}","l");
    //if(kBar) lgt[0]->SetTextSize(50);
    //lgt[0]->Draw("same");
    
    p3->cd();
    TFile *f7GT = TFile::Open("./input/sbeta_GT_7m.root");
    TH1F *h7GT =  (TH1F*)f7GT->Get("sbetaGT_smf1");
    h7GT->SetLineWidth(0);
    
    h7GT->GetXaxis()->SetRangeUser(6.0,8.5);
    h7GT->GetXaxis()->SetTitle("");
    h7GT->GetXaxis()->SetLabelOffset(0.04);
    h7GT->GetXaxis()->SetLabelSize(0.07);

    h7GT->GetYaxis()->SetLabelOffset(0.02);
    h7GT->GetYaxis()->SetLabelSize(0.07);
    h7GT->GetYaxis()->SetTickLength(0.02);
    h7GT->GetYaxis()->SetTitleSize(0.07);
    h7GT->GetYaxis()->SetTitleOffset(0.60);
    h7GT->GetYaxis()->SetRangeUser(0.01,1.6);
    
    if(!kBar) h7GT->Draw("hist Y+");
    else h7GT->Draw("hbar");
    //lgt[1] = new TLegend(0.5,0.65,0.8,0.8);
	//lgt[1]->AddEntry(h7GT,"^{134}In J^{#pi}: 7^{-}","l");
	//if(kBar) lgt[1]->SetTextSize(50);
    //lgt[1]->Draw("same");

    //c1->SaveAs("temp.pdf");


//    if(kPrint){
//        for (int ib=1;ib<=nBins;ib++)
//            if(hGS->GetBinContent(ib)>0.01) cout << std::fixed << std::setprecision(2) 
//                                               << hGS->GetBinCenter(ib) << "\t" 
//                                               << hGS->GetBinContent(ib) << "\t" 
//                                               << h1561->GetBinContent(ib) << "\t" 
//                                               << h854->GetBinContent(ib) << "\n";
//    }
    //p1->BuildLegend();
    c1->SaveAs("Ibeta.pdf");

    return;
}

void OldHistErrMaker(const char *filename_=NULL, bool kBar=false, bool kShade=false, bool kPrint=false){

    gROOT->Reset();
    int hScale = 1;
    
    if(kShade) gROOT->SetBatch(true);
    double icount=0, Pn=0.60;
    double t = 0.140;

    if(filename_==NULL) { cout << "No File Name Specified" << endl; return;}
    gStyle->SetOptStat(0);
    gStyle->SetEndErrorSize(7);
    double xlo=-8.5;
    double xhi=-6;
    double width=0.1;
    int nBins = (xhi-xlo)/width;
    
    TH1F *h1561 = new TH1F("h1561","",nBins,xlo,xhi);
    TH1F *h854 = new TH1F("h854","",nBins,xlo,xhi);
    TH1F *hGS = new TH1F("hGS","",nBins,xlo,xhi);

    TGraphErrors *g1561 = new TGraphErrors(); g1561->SetName("g1561");
    TGraphErrors *g854 = new TGraphErrors(); g854->SetName("g854");
    TGraphErrors *gGS = new TGraphErrors(); gGS->SetName("gGS");

    THStack *hs = new THStack("hs","");
    FileReader fr(filename_);

    vector<vector<string>> lInfo = fr.LoadFile();
    int nlines = lInfo.size();

    vector <double> itot;
    vector <double> itoterr;

    double tempE = 0;
    for (int i=0;i<nlines;i++){
        int ncol = lInfo.at(i).size();
        double Ex = stod(lInfo.at(i).at(0));
        double gsA = stod(lInfo.at(i).at(1));
        double gam9 = stod(lInfo.at(i).at(3));
        double gam3 = stod(lInfo.at(i).at(5));
        
        double gsE = stod(lInfo.at(i).at(2));
        double gam9E = stod(lInfo.at(i).at(4));
        double gam3E = stod(lInfo.at(i).at(6));
        
        if(Ex>6.0&&Ex<8.5){
            int binID = hGS->FindBin(Ex*-1.0);
            double binC = hGS->GetBinCenter(binID);
        
            if(gsA>0.01){
                hGS->SetBinContent(binID,gsA);
                tempE = stod(lInfo.at(i).at(2))/stod(lInfo.at(i).at(1));
                gGS->SetPoint(i,binC,gsA); //gGS->SetPointError(i,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gsA);
                //gGS->SetPointError(i,0.0,stod(lInfo.at(i).at(2)));
                gGS->SetPointError(i,0.0,gsE);
            }
            if(gam9>0.01){
                h1561->SetBinContent(binID,gam9);
                tempE = stod(lInfo.at(i).at(4))/stod(lInfo.at(i).at(3));
                g1561->SetPoint(i,binC,gsA+gam3+gam9); //g1561->SetPointError(i,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gam9);
                g1561->SetPointError(i,0.0,gam9E);
            }
            if(gam3>0.01){
                h854->SetBinContent(binID,gam3);
                tempE = stod(lInfo.at(i).at(6))/stod(lInfo.at(i).at(5));
                g854->SetPoint(i,binC,gsA+gam3); //g854->SetPointError(i,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gam3);
                g854->SetPointError(i,0.0,gam3E);
            }
        }

        itot.push_back(gsA+gam9+gam3);
        itoterr.push_back(sqrt(gsE*gsE+gam9E*gam9E+gam3E*gam3E));
        icount += gsA+gam9+gam3;
        //lf.push_back(FermiIntegral(50,(14.6-Ex)*1000));
        //lferr.push_back(0);
    }

    icount = icount/Pn;
    
    if(kPrint){
    for (int i=0;i<itot.size();i++){
        double Ex = stod(lInfo.at(i).at(0));
        double tof = calcTOF(Ex-3.62);
        double Eerr = sqrt(pow(1.2/tof,2)+ pow(3/100.,2))*Ex;
        
        double lf = FermiIntegral(50,(14.6-Ex)*1000.);
        double lf_hi = FermiIntegral(50,(14.6-(Ex+Eerr))*1000.);
        double lf_lo = FermiIntegral(50,(14.6-(Ex-Eerr))*1000.);
        
        double th = t/(itot.at(i)/icount);
        double th_hi = t/((itot.at(i)+itoterr.at(i))/icount);
        double th_lo = t/((itot.at(i)-itoterr.at(i))/icount);
        
        
        double bgt = 6144./(pow(10,lf)*th);
        double bgt_hi = 6114./(pow(10,lf_hi*th_hi));
        double bgt_lo = 6114./(pow(10,lf_lo*th_lo));
        
        printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf_hi+log10(th_hi), bgt_hi, lf_lo+log10(th_lo), bgt_lo);
        //printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf_hi+log10(th), bgt_hi, lf_lo+log10(th), bgt_lo);
        //printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf+log10(th_hi), bgt_hi, lf+log10(th_lo), bgt_lo);
    }
    }

    TCanvas *c1 = new TCanvas("c1","c1",1000,700);
    TPad *p1,*p2, *p3;
    if (!kBar){
        p1 = new TPad("p1","p1",0,0,1,0.5);
        p2 = new TPad("p2","p2",0,0.5,1,0.75);
        p3 = new TPad("p3","p3",0,0.75,1,1);
    } else{
        p1 = new TPad("p1","p1",0,0,0.5,1);
        p2 = new TPad("p2","p2",0.5,0,0.75,1);
        p3 = new TPad("p3","p3",0.75,0,1,1);
    }
    p1->Draw(); p2->Draw(); p3->Draw();
    


    p1->cd();

    hGS->SetFillColor(lc[0]); h1561->SetFillColor(lc[1]); h854->SetFillColor(lc[2]);
    hGS->SetLineColor(lc[0]); h1561->SetLineColor(lc[1]); h854->SetLineColor(lc[2]);
    hGS->SetLineWidth(0); h1561->SetLineWidth(0); h854->SetLineWidth(0);


    hs->Add(hGS); hs->Add(h854); hs->Add(h1561);

    if(!kBar) hs->Draw();
    else hs->Draw("hbar");
    hs->GetXaxis()->SetRangeUser(6.0,8.5);
    hs->GetXaxis()->SetNdivisions(506);
    for(int il=1;il<=6;il++) hs->GetXaxis()->ChangeLabel(-il,90,-1,-1,-1,-1,Form("%.1f",6.0+(il-1)*0.5));
    
    hs->GetYaxis()->SetNdivisions(503);
    for(int il=1;il<=3;il++) hs->GetYaxis()->ChangeLabel(il,90);
    
    if(!kBar) hs->Draw();
    else hs->Draw("hbar");

    gGS->SetMarkerStyle(20); g1561->SetMarkerStyle(20); g854->SetMarkerStyle(20);
    gGS->SetMarkerSize(0.7); g1561->SetMarkerSize(0.7); g854->SetMarkerSize(0.7);
    gGS->SetLineWidth(2); g1561->SetLineWidth(2); g854->SetLineWidth(2);
    gGS->SetLineColor(ld[0]); g1561->SetLineColor(ld[1]); g854->SetLineColor(ld[2]);
    gGS->SetMarkerColor(ld[0]); g1561->SetMarkerColor(ld[1]); g854->SetMarkerColor(ld[2]);

    gGS->Draw("same P"); g1561->Draw("same P"); g854->Draw("same P");

    TLegend *l1 = new TLegend(0.65,0.6,0.85,0.9,"Feeding");
    l1->AddEntry(hGS,"7/2(GS)","f"); l1->AddEntry(h854,"3/2 (854)","f"); l1->AddEntry(h1561,"9/2 (1561)","f");
    if(kBar) l1->SetTextSize(40);
    l1->Draw("same");

    TLegend *lgt[2];
    
        
    p2->cd();
    TFile *f6GT = TFile::Open("./input/sbeta_GT_6m.root");
    TH1F *h6GT =  (TH1F*)f6GT->Get("sbetaGT_smf1");
    h6GT->SetLineWidth(0);
    
    h6GT->GetXaxis()->SetRangeUser(6.0,8.5);
    h6GT->GetXaxis()->SetTitle("");
    h6GT->GetXaxis()->SetLabelOffset(0.04);
    h6GT->GetXaxis()->SetLabelSize(0.07);
    
    h6GT->GetYaxis()->SetLabelOffset(0.02);
    h6GT->GetYaxis()->SetLabelSize(0.07);
    h6GT->GetYaxis()->SetTickLength(0.02);
    h6GT->GetYaxis()->SetTitleSize(0.07);
    h6GT->GetYaxis()->SetTitleOffset(0.60);
    h6GT->GetYaxis()->SetRangeUser(0.01,1.6);
    


    if(!kBar) h6GT->Draw("hist Y+");
    else h6GT->Draw("hbar");
    lgt[0] = new TLegend(0.5,0.65,0.8,0.8);
	lgt[0]->AddEntry(h6GT,"^{134}In J^{#pi}: 6^{-}","l");
    if(kBar) lgt[0]->SetTextSize(50);
    lgt[0]->Draw("same");
    
    p3->cd();
    TFile *f7GT = TFile::Open("./input/sbeta_GT_7m.root");
    TH1F *h7GT =  (TH1F*)f7GT->Get("sbetaGT_smf1");
    h7GT->SetLineWidth(0);
    
    h7GT->GetXaxis()->SetRangeUser(6.0,8.5);
    h7GT->GetXaxis()->SetTitle("");
    h7GT->GetXaxis()->SetLabelOffset(0.04);
    h7GT->GetXaxis()->SetLabelSize(0.07);

    h7GT->GetYaxis()->SetLabelOffset(0.02);
    h7GT->GetYaxis()->SetLabelSize(0.07);
    h7GT->GetYaxis()->SetTickLength(0.02);
    h7GT->GetYaxis()->SetTitleSize(0.07);
    h7GT->GetYaxis()->SetTitleOffset(0.60);
    h7GT->GetYaxis()->SetRangeUser(0.01,1.6);
    
    if(!kBar) h7GT->Draw("hist Y+");
    else h7GT->Draw("hbar");
    lgt[1] = new TLegend(0.5,0.65,0.8,0.8);
	lgt[1]->AddEntry(h7GT,"^{134}In J^{#pi}: 7^{-}","l");
	if(kBar) lgt[1]->SetTextSize(50);
    lgt[1]->Draw("same");

//    TFile *fOut = new TFile("IBeta.root","update");
//    hs->Write();
//    gGS->Write();
//    g854->Write();
//    g1561->Write();
//    l1->Write();
//    
//    fOut->Close();

    if(kPrint){
        for (int ib=1;ib<=nBins;ib++)
            if(hGS->GetBinContent(ib)>0.01) cout << std::fixed << std::setprecision(2) 
                                               << hGS->GetBinCenter(ib) << "\t" 
                                               << hGS->GetBinContent(ib) << "\t" 
                                               << h1561->GetBinContent(ib) << "\t" 
                                               << h854->GetBinContent(ib) << "\n";
    }

    return;
}

void GraphErrMaker(const char *filename_=NULL, bool kPrint=false, bool kBar=false, bool kShade=false){

    gROOT->Reset();
    gStyle->SetBarWidth(0.4);
    
    if(kShade) gROOT->SetBatch(true);
    double icount=0, Pn=0.89;
    double t = 0.140;

    if(filename_==NULL) { cout << "No File Name Specified" << endl; return;}
    gStyle->SetOptStat(0);
    //gStyle->SetEndErrorSize(7);
  
    const char *gnames[3] = {"GS","854","1561"};

    TGraphErrors *gvals[3];
    TGraph *ggvals[3];
    
    for(int i=0;i<3;i++){
        gvals[i] = new TGraphErrors();
        gvals[i]->SetName(gnames[i]);
        ggvals[i] = new TGraph();
        ggvals[i]->SetName(Form("g%d",i));
    
        ggvals[i]->SetPoint(0,0,0);
    
    }
    
    
    FileReader fr(filename_);

    double elow=6.0, ehi=8.5;

    vector<vector<string>> lInfo = fr.LoadFile();
    int nlines = lInfo.size();

    vector <double> itot;
    vector <double> itoterr;

    double tempE = 0;
    int ip=0;
    
    

    for (int i=0;i<nlines;i++){
        int ncol = lInfo.at(i).size();
        double Ex = stod(lInfo.at(i).at(0));
        double gsA = stod(lInfo.at(i).at(1));
        double gam9 = stod(lInfo.at(i).at(3));
        double gam3 = stod(lInfo.at(i).at(5));
        
        double gsE = stod(lInfo.at(i).at(2));
        double gam9E = stod(lInfo.at(i).at(4));
        double gam3E = stod(lInfo.at(i).at(6));
        
        if(Ex>elow&&Ex<ehi){
            
            if(gsA>0.01){
                tempE = stod(lInfo.at(i).at(2))/stod(lInfo.at(i).at(1));
                gvals[0]->SetPoint(ip,-Ex,gsA); //gvals[0]->SetPointError(ip,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gsA);
                //gvals[0]->SetPointError(ip,0.0,stod(lInfo.at(i).at(2)));
                gvals[0]->SetPointError(ip,calcErr(Ex-3.62-1.561)/2.-0.0025,gsE);
                ggvals[0]->SetPoint(4*ip+1,-Ex+calcErr(Ex-3.62-1.561)/2.,0);
                ggvals[0]->SetPoint(4*ip+2,-Ex+calcErr(Ex-3.62-1.561)/2.,gsA);
                ggvals[0]->SetPoint(4*ip+3,-Ex-calcErr(Ex-3.62-1.561)/2.,gsA);
                ggvals[0]->SetPoint(4*ip+4,-Ex-calcErr(Ex-3.62-1.561)/2.,0);
                        
                tempE = stod(lInfo.at(i).at(6))/stod(lInfo.at(i).at(5));
                gvals[1]->SetPoint(ip,-Ex,gsA+gam3); //gvals[1]->SetPointError(ip,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gam3);
                gvals[1]->SetPointError(ip,calcErr(Ex-3.62-1.561)/2.-0.0025,gam3E);
                ggvals[1]->SetPoint(4*ip+1,-Ex+calcErr(Ex-3.62-1.561)/2.,0);
                ggvals[1]->SetPoint(4*ip+2,-Ex+calcErr(Ex-3.62-1.561)/2.,gsA+gam3);
                ggvals[1]->SetPoint(4*ip+3,-Ex-calcErr(Ex-3.62-1.561)/2.,gsA+gam3);
                ggvals[1]->SetPoint(4*ip+4,-Ex-calcErr(Ex-3.62-1.561)/2.,0);
                
                tempE = stod(lInfo.at(i).at(4))/stod(lInfo.at(i).at(3));
                gvals[2]->SetPoint(ip,-Ex,gsA+gam3+gam9); //gvals[2]->SetPointError(ip,0.0,sqrt(pow(tempE,2)+pow(0.05,2))*gam9);
                gvals[2]->SetPointError(ip,calcErr(Ex-3.62-1.561)/2.-0.0025,gam9E);
                ggvals[2]->SetPoint(4*ip+1,-Ex+calcErr(Ex-3.62-1.561)/2.,0);
                ggvals[2]->SetPoint(4*ip+2,-Ex+calcErr(Ex-3.62-1.561)/2.,gsA+gam3+gam9);
                ggvals[2]->SetPoint(4*ip+3,-Ex-calcErr(Ex-3.62-1.561)/2.,gsA+gam3+gam9);
                ggvals[2]->SetPoint(4*ip+4,-Ex-calcErr(Ex-3.62-1.561)/2.,0);
            
            }
            ip++;
        }

        itot.push_back(gsA+gam9+gam3);
        itoterr.push_back(sqrt(gsE*gsE+gam9E*gam9E+gam3E*gam3E));
        icount += gsA+gam9+gam3;
        //lf.push_back(FermiIntegral(50,(14.6-Ex)*1000));
        //lferr.push_back(0);
    }

    icount = icount/Pn;
    
    if(kPrint){
    for (int i=0;i<itot.size();i++){
        double Ex = stod(lInfo.at(i).at(0));
        double tof = calcTOF(Ex-3.62);
        double Eerr = sqrt(pow(1.2/tof,2)+ pow(3/100.,2))*Ex;
        
        double lf = FermiIntegral(50,(14.6-Ex)*1000.);
        double lf_hi = FermiIntegral(50,(14.6-(Ex+Eerr))*1000.);
        double lf_lo = FermiIntegral(50,(14.6-(Ex-Eerr))*1000.);
        
        double th = t/(itot.at(i)/icount);
        double th_hi = t/((itot.at(i)+itoterr.at(i))/icount);
        double th_lo = t/((itot.at(i)-itoterr.at(i))/icount);
        
        
        double bgt = 6144./(pow(10,lf)*th);
        double bgt_hi = 6144./(pow(10,lf_hi*th_hi));
        double bgt_lo = 6144./(pow(10,lf_lo*th_lo));
        
        printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf_hi+log10(th_hi), bgt_hi, lf_lo+log10(th_lo), bgt_lo);
        //printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf_hi+log10(th), bgt_hi, lf_lo+log10(th), bgt_lo);
        //printf("%.3f\t%.2f\t%.4f\t%.2f\t%.4f\t%.2f\t%.4f\n", Ex, lf+log10(th), bgt, lf+log10(th_hi), bgt_hi, lf+log10(th_lo), bgt_lo);
    }
    }

    TCanvas *c1 = new TCanvas("c1","c1",900,800);
    TPad *p1,*p2, *p3;
    if (!kBar){
        p1 = new TPad("p1","p1",0,0,1,0.5);
        p2 = new TPad("p2","p2",0,0.5,1,0.75);
        p3 = new TPad("p3","p3",0,0.75,1,1);
    } else{
        p1 = new TPad("p1","p1",0,0,0.5,1);
        p2 = new TPad("p2","p2",0.5,0,0.75,1);
        p3 = new TPad("p3","p3",0.75,0,1,1);
    }
    
    p1->Draw(); p2->Draw(); p3->Draw();
    
    p1->cd();

    for(int i=0;i<3;i++){
        ggvals[i]->SetMarkerStyle(1);    
        ggvals[i]->SetMarkerSize(0);     
        ggvals[i]->SetLineWidth(0);      
        ggvals[i]->SetLineColor(ld[i]);  
        ggvals[i]->SetMarkerColor(lc[i]);
        ggvals[i]->SetFillColor(lc[i]);  
        ggvals[i]->GetXaxis()->SetLimits(-ehi,-elow);
    }
    
    
    ggvals[2]->GetXaxis()->SetNdivisions(506);
    ggvals[2]->GetXaxis()->SetLabelOffset(0.015);
    ggvals[2]->GetXaxis()->SetLabelSize(0.06);
    for(int il=1;il<=6;il++) ggvals[2]->GetXaxis()->ChangeLabel(-il,90,-1,-1,-1,-1,Form("%.1f",6.0+(il-1)*0.5));
    
    ggvals[2]->GetYaxis()->SetLimits(0,12000);
    ggvals[2]->GetYaxis()->SetRangeUser(10,12000);
    ggvals[2]->GetYaxis()->SetLabelOffset(0.015);
    ggvals[2]->GetYaxis()->SetNdivisions(503);
    for(int il=1;il<=3;il++) ggvals[2]->GetYaxis()->ChangeLabel(il,90);
    
    ggvals[2]->GetYaxis()->SetRangeUser(10,12000);
    ggvals[2]->Draw("AF");

    ggvals[1]->Draw("F same");
    ggvals[0]->Draw("F same");
    
    
    
    gvals[0]->SetLineWidth(2); gvals[1]->SetLineWidth(2); gvals[2]->SetLineWidth(2);
    gvals[0]->SetLineColor(kRed+3); gvals[1]->SetLineColor(kRed-4); gvals[2]->SetLineColor(kRed+1);

    gvals[0]->SetFillStyle(0); gvals[1]->SetFillStyle(0); gvals[2]->SetFillStyle(0);

    gvals[2]->Draw("5 same");
    gvals[1]->Draw("5 same"); 
    gvals[0]->Draw("5 same");

    gPad->RedrawAxis();

    if (kPrint){
        for (int jj=1;jj<gvals[0]->GetN(); jj++) cout << gvals[0]->GetPointX(jj) << "\t" << gvals[1]->GetPointX(jj) << "\t" << gvals[2]->GetPointX(jj) << endl;
    }

    TLegend *l1 = new TLegend(0.65,0.6,0.85,0.85,"Feeding");
    l1->AddEntry(ggvals[0],"7/2(GS)","pf"); l1->AddEntry(ggvals[1],"3/2 (854)","pf"); l1->AddEntry(ggvals[2],"9/2 (1561)","pf");
    if(kBar) l1->SetTextSize(40);
    l1->Draw("same");

    //TLegend *lgt[2];
            
    p2->cd();
    TFile *f6GT = TFile::Open("./input/bgt_GT_6m.root");
    TH1F *h6GT =  (TH1F*)f6GT->Get("sbetaGT_smf1");
    h6GT->SetLineWidth(0);
    
    h6GT->GetXaxis()->SetRangeUser(6.0,8.5);
    h6GT->GetXaxis()->SetTitle("");
    h6GT->GetXaxis()->SetLabelOffset(0.04);
    h6GT->GetXaxis()->SetLabelSize(0.07);
    
    h6GT->GetYaxis()->SetLabelOffset(0.02);
    h6GT->GetYaxis()->SetLabelSize(0.07);
    h6GT->GetYaxis()->SetTickLength(0.02);
    h6GT->GetYaxis()->SetTitleSize(0.07);
    h6GT->GetYaxis()->SetTitleOffset(0.60);
    h6GT->GetYaxis()->SetRangeUser(0.01,1.6);
    


    if(!kBar) h6GT->Draw("hist Y+");
    else h6GT->Draw("hbar");
    //lgt[0] = new TLegend(0.5,0.65,0.8,0.8);
	//lgt[0]->AddEntry(h6GT,"^{134}In J^{#pi}: 6^{-}","l");
    //if(kBar) lgt[0]->SetTextSize(50);
    //lgt[0]->Draw("same");
    
    p3->cd();
    TFile *f7GT = TFile::Open("./input/bgt_GT_7m.root");
    TH1F *h7GT =  (TH1F*)f7GT->Get("sbetaGT_smf1");
    h7GT->SetLineWidth(0);
    
    h7GT->GetXaxis()->SetRangeUser(6.0,8.5);
    h7GT->GetXaxis()->SetTitle("");
    h7GT->GetXaxis()->SetLabelOffset(0.04);
    h7GT->GetXaxis()->SetLabelSize(0.07);

    h7GT->GetYaxis()->SetLabelOffset(0.02);
    h7GT->GetYaxis()->SetLabelSize(0.07);
    h7GT->GetYaxis()->SetTickLength(0.02);
    h7GT->GetYaxis()->SetTitleSize(0.07);
    h7GT->GetYaxis()->SetTitleOffset(0.60);
    h7GT->GetYaxis()->SetRangeUser(0.01,1.6);
    
    if(!kBar) h7GT->Draw("hist Y+");
    else h7GT->Draw("hbar");
    //lgt[1] = new TLegend(0.5,0.65,0.8,0.8);
	//lgt[1]->AddEntry(h7GT,"^{134}In J^{#pi}: 7^{-}","l");
	//if(kBar) lgt[1]->SetTextSize(50);
    //lgt[1]->Draw("same");

    //c1->SaveAs("temp.pdf");


//    if(kPrint){
//        for (int ib=1;ib<=nBins;ib++)
//            if(hGS->GetBinContent(ib)>0.01) cout << std::fixed << std::setprecision(2) 
//                                               << hGS->GetBinCenter(ib) << "\t" 
//                                               << hGS->GetBinContent(ib) << "\t" 
//                                               << h1561->GetBinContent(ib) << "\t" 
//                                               << h854->GetBinContent(ib) << "\n";
//    }
    //p1->BuildLegend();
    c1->SaveAs("Ibeta.pdf");

    return;
}
