{
TF1 *f = new TF1("f",bspectrum,1,10,2);
f->SetNpx(1000);
f->SetParameters(50,2.5);
f->Draw();
}
