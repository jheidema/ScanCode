{
TF1 *f = new TF1("f",rbspectrum,0.1,5,2);
f->SetNpx(1000);
f->SetParameters(50,7);
f->Draw();
}
