#include "TAxis.h"
#include "TF1.h"

TGaxis *MakeAxis(double xmin, double xmax, double ymin, double ymax){

  TGaxis *eAxis;
  if (ymin==ymax) {
	TF1 *nEnergy = new TF1("nEnergy","0.5*939.6*pow(100.0/x/29.98,2)",xmin,xmax); 
	eAxis = new TGaxis(xmin,ymin,xmax,ymax,"nEnergy",10,"");
  }else if (xmin==xmax) {
  	TF1 *nEnergy = new TF1("nEnergy","0.5*939.6*pow(100.0/x/29.98,2)",ymin,ymax);
	eAxis = new TGaxis(xmin,ymin,xmax,ymax,"nEnergy",10,"");
  }
  return eAxis;
}
