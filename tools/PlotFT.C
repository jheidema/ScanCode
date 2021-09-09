#include "TGraphAsymmErrors.h"


void PlotFT(){
gStyle->SetEndErrorSize(4.5);

//TH1F *h[4];
TGraphAsymmErrors *g[4];
int pn[4] = {60,70,80,90};

//const char **fnames[4][15] = {"file1.txt","file2.txt","file3.txt","file4.txt"};

TCanvas *c1 = new TCanvas();
c1->Divide(2,2);

for (int i=0;i<4;i++){

    c1->cd(i+1);
    //fin.open(fname[i],std::ifstream::in);
    //fin >> nlines ;
    //for (int j=0;j<nlines;j++){
    //    fin >> 
    //}


    //h[i] = new TH1F("h1",Form("P_{n}%d",pn[i]),80,5.5,9.5);
    g[i] = new TGraphAsymmErrors(Form("logft_%d.txt",pn[i]),"%lg %lg %lg %lg");
    g[i]->SetTitle(Form("P_{n} %d%%", pn[i]));
    g[i]->GetXaxis()->SetRangeUser(5.5,9.5);
    g[i]->SetMarkerStyle(20);
    g[i]->SetMarkerSize(1.2);

    g[i]->GetXaxis()->SetTitle("Ex");
    g[i]->GetXaxis()->SetTitleSize(0.06);
    g[i]->GetXaxis()->SetTitleOffset(0.8);
    g[i]->GetXaxis()->SetLabelSize(0.05);
    
    g[i]->GetYaxis()->SetTitle("log #font[12]{ft}");
    g[i]->GetYaxis()->SetTitleSize(0.06);
    g[i]->GetYaxis()->SetTitleOffset(0.65);
    g[i]->GetYaxis()->SetLabelSize(0.05);
    
    
    g[i]->Draw("AP");
}



//7.81	5.47	-0.24	0.3		5.4	-0.24	0.3		5.34	-0.24	0.3		5.29	-0.24	0.3
//7.37	5.13	-0.11	0.12		5.07	-0.12	0.11		5.01	-0.12	0.11		4.96	-0.12	0.11
//7.18	5.12	-0.11	0.11		5.05	-0.11	0.12		5	-0.12	0.11		4.94	-0.11	0.12
//6.88	5.14	-0.11	0.1		5.07	-0.11	0.11		5.01	-0.11	0.11		4.96	-0.11	0.11


}
