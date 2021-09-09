
void makeHists(){

  gROOT->SetBatch(1);
  //  gStyle->SetCanvasPreferGL(true);

  habs0_j4->GetXaxis()->SetRangeUser(4.5,10);
  habs1_j4->GetXaxis()->SetRangeUser(4.5,10);
  habs2_j4->GetXaxis()->SetRangeUser(4.5,10);
  habs0_j5->GetXaxis()->SetRangeUser(4.5,10);
  habs1_j5->GetXaxis()->SetRangeUser(4.5,10);
  habs2_j5->GetXaxis()->SetRangeUser(4.5,10);
  habs0_j6->GetXaxis()->SetRangeUser(4.5,10);
  habs1_j6->GetXaxis()->SetRangeUser(4.5,10);
  habs2_j6->GetXaxis()->SetRangeUser(4.5,10);
  habs0_j7->GetXaxis()->SetRangeUser(4.5,10);
  habs1_j7->GetXaxis()->SetRangeUser(4.5,10);
  habs2_j7->GetXaxis()->SetRangeUser(4.5,10);
  habs0_j8->GetXaxis()->SetRangeUser(4.5,10);
  habs1_j8->GetXaxis()->SetRangeUser(4.5,10);
  habs2_j8->GetXaxis()->SetRangeUser(4.5,10);

  habs0_j4->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs1_j4->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs2_j4->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs0_j5->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs1_j5->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs2_j5->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs0_j6->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs1_j6->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs2_j6->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs0_j7->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs1_j7->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs2_j7->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs0_j8->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs1_j8->GetYaxis()->SetRangeUser(1E-5,1E4);
  habs2_j8->GetYaxis()->SetRangeUser(1E-5,1E4);

  habs0_j4->SetFillColorAlpha(kBlue,0.5);
  habs1_j4->SetFillColorAlpha(kGreen,0.5);
  habs2_j4->SetFillColorAlpha(kRed,0.5);
  
  habs0_j5->SetFillColorAlpha(kBlue,0.5);
  habs1_j5->SetFillColorAlpha(kGreen,0.5);
  habs2_j5->SetFillColorAlpha(kRed,0.5);
  
  habs0_j6->SetFillColorAlpha(kBlue,0.5);
  habs1_j6->SetFillColorAlpha(kGreen,0.5);
  habs2_j6->SetFillColorAlpha(kRed,0.5);
  
  habs0_j7->SetFillColorAlpha(kBlue,0.5);
  habs1_j7->SetFillColorAlpha(kGreen,0.5);
  habs2_j7->SetFillColorAlpha(kRed,0.5);
  
  habs0_j8->SetFillColorAlpha(kBlue,0.5);
  habs1_j8->SetFillColorAlpha(kGreen,0.5);
  habs2_j8->SetFillColorAlpha(kRed,0.5);


  TCanvas *c1 = new TCanvas("c1","c1",1000,800);
  c1->SetLogy();

  habs0_j5->GetYaxis()->SetRangeUser(1E-13,1E-2);
  //habs0_j5->GetXaxis()->SetRangeUser(1E-8,1E5);
  habs0_j5->Draw();
  habs1_j5->Draw("same");
  habs2_j5->Draw("same");


  c1->SaveAs("output/images/5pOverlay.png");
  
  int nbiny = habs0_j5->GetNbinsX();
  double ymin = habs0_j5->GetBinLowEdge(1);
  double ymax = habs0_j5->GetBinLowEdge(nbiny)+habs0_j5->GetBinWidth(nbiny);

  habs0_j6->GetYaxis()->SetRangeUser(1E-13,1E-2);
  
  habs0_j6->Draw();
  habs1_j6->Draw("same");
  habs2_j6->Draw("same");
  c1->SaveAs("output/images/6pOverlay.png");

  TH2F *h5 = new TH2F("h5","h5",3,0,3,nbiny,ymin,ymax);
  TH2F *h6 = new TH2F("h6","h6",3,0,3,nbiny,ymin,ymax);
int bID;
  for(int i=1;i<=nbiny;i++){
   bID = h5->FindBin(0,habs0_j5->GetBinCenter(i));
   h5->SetBinContent(bID,habs0_j5->GetBinContent(i)*1E10);
   h6->SetBinContent(bID,habs0_j6->GetBinContent(i)*1E10);

   bID = h5->FindBin(1,habs0_j5->GetBinCenter(i));
   h5->SetBinContent(bID,habs1_j5->GetBinContent(i)*1E10);
   h6->SetBinContent(bID,habs1_j6->GetBinContent(i)*1E10);

   bID = h5->FindBin(2,habs0_j5->GetBinCenter(i));
   h5->SetBinContent(bID,habs2_j5->GetBinContent(i)*1E10);
   h6->SetBinContent(bID,habs2_j6->GetBinContent(i)*1E10);

  }



  return;
}
