#ifndef dummyFunc_hpp
#define dummyFunc_hpp


extern double tofWcorr(double *x, double *p);
extern double g_peak(double *x, double *p);
extern double gauss(double *x, double *p);
extern double agauss(double *x, double *p);
extern double normal(double *x, double *p);
extern double lorentz(double *x, double *p);
extern double alorentz(double *x, double *p);
extern double alandau(double *x, double *p);
extern double hypBkgd(double *x, double *p);
extern double pieceBkgd(double *x, double *p);
extern double gammaBkgd(double *x, double *p);
extern double FitBkgd(double *x, double *p);
extern double FullBkgd(double *x, double *p);
extern double MadeyQ(double *x, double *p);
extern double MadeyQ(double x, double *p);
extern double QDCEdge(double *x, double *p);
extern double RelHPGe_Eff(double *x, double *p);
extern double AbsHPGe_Eff(double *x, double *p);
extern double AbsHPGe_Eff(double x);

#endif