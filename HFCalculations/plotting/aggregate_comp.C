void aggregate_comp(void){

TFile *file1 = new TFile("../output/files/Doorway.root");

TGraph *g0[3];

g0[0] = (TGraph*)file1->Get("g0_s6");
g0[1] = (TGraph*)file1->Get("g1_s6");
g0[2] = (TGraph*)file1->Get("g2_s6");

file1->Close();

TFile *file2 = new TFile("./hfhists.root");

TGraph *ghf[3];
ghf[0] = (TGraph*)file2->Get("h0");
ghf[1] = (TGraph*)file2->Get("h1");
ghf[2] = (TGraph*)file2->Get("h2");

for(int i=0;i<3;i++){
    ghf[i]->SetLineStyle(1);
    ghf[i]->SetLineWidth(3);
} 

TCanvas *c1 = new TCanvas();

g0[2]->GetXaxis()->SetLimits(-8.5,-6);
g0[2]->Draw("AF");
g0[1]->Draw("F same");
g0[0]->Draw("F same");

ghf[0]->Draw("L same");
ghf[1]->Draw("L same");
ghf[2]->Draw("L same");

gPad->RedrawAxis();

}