#include "FileReader.C"

using namespace std;

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
    double width=0.15;
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
    l1->Draw("same");

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

void HistErrMaker(const char *filename_=NULL, bool kPrint=false){
    
    if(filename_==NULL) { cout << "No File Name Specified" << endl; return;}
    gStyle->SetOptStat(0);
    gStyle->SetEndErrorSize(8.5);
    double xlo=0;
    double xhi=15;
    double width=0.05;
    int nBins = (xhi-xlo)/width;
    TH1F *h1561 = new TH1F("h1561","",nBins,xlo,xhi);
    TH1F *h854 = new TH1F("h854","",nBins,xlo,xhi);
    TH1F *hGS = new TH1F("hGS","",nBins,xlo,xhi);

    TGraphErrors *g1561 = new TGraphErrors(); g1561->SetName("g1561");
    TGraphErrors *g854 = new TGraphErrors(); g854->SetName("g854");
    TGraphErrors *gGS = new TGraphErrors(); gGS->SetName("gGS");

    THStack *hs = new THStack("hs","Stacked BGT");
    FileReader fr(filename_);

    vector<vector<string>> lInfo = fr.LoadFile();
    int nlines = lInfo.size();
    
    for (int i=0;i<nlines;i++){
        int ncol = lInfo.at(i).size();
        double Ex = stod(lInfo.at(i).at(0));
        int binID = hGS->FindBin(Ex);
        double binC = hGS->GetBinCenter(binID);
        double gsA = stod(lInfo.at(i).at(1));
        double gam1A = stod(lInfo.at(i).at(3));
        double gam2A = stod(lInfo.at(i).at(5));
        
        hGS->SetBinContent(binID,gsA);
        gGS->SetPoint(i,binC,gsA); gGS->SetPointError(i,0.0,stod(lInfo.at(i).at(2)));
        
        h1561->SetBinContent(binID,gam1A);
        g1561->SetPoint(i,binC,gsA+gam1A); g1561->SetPointError(i,0.0,stod(lInfo.at(i).at(4)));
        
        h854->SetBinContent(binID,gam2A);
        g854->SetPoint(i,binC,gsA+gam1A+gam2A); g854->SetPointError(i,0.0,stod(lInfo.at(i).at(6)));
        
    }

    hGS->SetFillColor(kGreen+3); h1561->SetFillColor(kBlue-6); h854->SetFillColor(kRed-6);
    hGS->SetLineColor(kGreen+3); h1561->SetLineColor(kBlue-6); h854->SetLineColor(kRed-6);

    hs->Add(hGS); hs->Add(h1561); hs->Add(h854);
    hs->Draw();

    gGS->SetMarkerStyle(1); g1561->SetMarkerStyle(1); g854->SetMarkerStyle(1);
    gGS->SetLineWidth(2); g1561->SetLineWidth(2); g854->SetLineWidth(2);
    gGS->SetLineColor(kGreen); g1561->SetLineColor(kCyan); g854->SetLineColor(kMagenta);
    gGS->SetMarkerColor(kGreen); g1561->SetMarkerColor(kCyan); g854->SetMarkerColor(kMagenta);

    gGS->Draw("same P"); g1561->Draw("same P"); g854->Draw("same P");

    TLegend *l1 = new TLegend(0.3,0.3,"Feeding");
    l1->AddEntry(hGS,"7/2(GS)","f"); l1->AddEntry(h1561,"9/2 (1561)","f"); l1->AddEntry(h854,"3/2 (854)","f");
    l1->Draw("same");

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

