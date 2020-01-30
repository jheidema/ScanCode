#include "FullSpecFunc.hpp"

#ifdef FullSpecFunc_hpp

//#include "fullFuncClass.hpp"
#include "TH1F.h"
#include "TF1.h"

#include "BkgdFunc.hpp"
//#include "HypBkgdFunc.C"
//#include "PieceBkgdFunc.C"
#include "SpectrumFunc.hpp"


using namespace std;

FullSpecFunc::FullSpecFunc(){};

FullSpecFunc::~FullSpecFunc(){};

bool FullSpecFunc::OpenFile(){
    
    fr.OpenFile(filename);  
}

void FullSpecFunc::GenerateSpecFunc(TH1F* hIn, bool kHyp){
    hIn->GetXaxis()->UnZoom();
    hIn->GetYaxis()->UnZoom();
    hIn->GetListOfFunctions()->Clear();
    
    fB = BkgdFunc(hIn,kHyp);
    bArray = fB->GetParameters();
    nBP = fB->GetNpar();
    cout << nBP << " background parameters\n";
    FullFuncClass tf(nBP,bArray);
    tf.PrintBkgdParams();
    
    cout << "Opening " << filename << endl;

    TF1 *fA = SpectrumFunc(filename,0.0,false);
    nFuncs = fA->GetParameter(0);

    for (int iF=0; iF<nFuncs; iF++){
        double t = fA->GetParameter(iF*nrp+1);
        double a = fA->GetParameter(iF*nrp+2);
        tf.InsertFunction(t,a);
    }    

    //std::cout << "Fully Loaded\n";
    //tf.GetBkgdParams();
    //cout << tf.GetNFuncs() << endl;

    fOut = new TF1("fOut",tf,0,800,0);
    fOut->SetNpx(2000); 

    //if(kDraw) {hIn->Draw("hist");fOut->Draw("same");}

    cout << "Good here\n";
    return;
}

#endif