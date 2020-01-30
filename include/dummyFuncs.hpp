#ifndef dummyFunc_hpp
#define dummyFunc_hpp


extern double tofWcorr(double *x, double *p);
extern double g_peak(double *x, double *p);
extern double hypBkgd(double *x, double *p);
extern double pieceBkgd(double *x, double *p);
extern double gammaBkgd(double *x, double *p);
extern double FullBkgd(double *x, double *p);
extern double MadeyQ(double *x, double *p);
extern double MadeyQ(double x, double *p);
extern double QDCEdge(double *x, double *p);
extern double RelHPGe_Eff(double *x, double *p);
extern double AbsHPGe_Eff(double *x, double *p);
extern double AbsHPGe_Eff(double x);

#endif