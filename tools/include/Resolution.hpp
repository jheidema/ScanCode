#include <cmath>

double mn = 939.7; // MeV/c^2
double c = 29.98; // cm/ns (Speed of Light)

double TwoDResolution(double *x, double *p){
	double dL = p[0];
	double dT = p[1];

	double E = x[1];
	double L = x[0];

	double factor = sqrt(mn/(2*E))/c;

	double dTe = factor*dL;
	double T = factor*L;

	return sqrt(4*(dT*dT)/(T*T)+4*dL*dL/(L*L))*E*1000.0;

}

double EResolution(double *x, double *p){
	double dL = p[0];
	double dT = p[1];
	double L  = p[2];

	double E = x[0];
	
	double factor = sqrt(mn/(2*E))/c;

	//double dTe = factor*dL;
	double T = factor*L;

	return sqrt(4.0*(dT*dT)/(T*T)+4.0*(dL*dL)/(L*L))*E*1000.0;

}
