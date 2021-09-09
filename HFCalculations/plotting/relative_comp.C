void relative_comp(void){

TFile *file1 = new TFile("../output/files/Doorway.root");

TGraph *g0[3];

g0[0] = (TGraph*)file1->Get("g0_s6");
g0[1] = (TGraph*)file1->Get("g1_s6");
g0[2] = (TGraph*)file1->Get("g2_s6");

file1->Close();

int np = g0[0]->GetN();

for(int i=1;i<np-1;i++){    
    double yval1 = g0[1]->GetPointY(i);
    double yval0 = g0[0]->GetPointY(i);
    g0[1]->SetPointY(i,yval1-yval0);
    g0[2]->SetPointY(i,1.0-yval1);
}


TFile *file2 = new TFile("./hfhists.root");

TGraph *ghf[3];
ghf[0] = (TGraph*)file2->Get("ghf6_0");
ghf[1] = (TGraph*)file2->Get("ghf6_1");
ghf[2] = (TGraph*)file2->Get("ghf6_2");

for(int i=0;i<3;i++){
    ghf[i]->SetLineStyle(9);
    ghf[i]->SetLineWidth(3);
    ghf[i]->SetLineColor(g0[i]->GetLineColor());
    g0[i]->SetLineWidth(3);
} 


TCanvas *c1 = new TCanvas();

g0[2]->GetXaxis()->SetLimits(-8.5,-6);
g0[2]->Draw("AL");
g0[1]->Draw("L same");
g0[0]->Draw("L same");

ghf[0]->Draw("L same");
ghf[1]->Draw("L same");
ghf[2]->Draw("L same");

gPad->RedrawAxis();

}