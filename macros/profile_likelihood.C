#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TSystem.h"
#include "iostream"
#include "RooProdPdf.h"
#include "RooAddPdf.h"
#include "RooMinimizer.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"

#include "TH1D.h"



using namespace RooFit;
using namespace std;
using namespace RooStats;

void profile_likelihood(){
    //READ WORKSPACE
    TFile *f = new TFile("workspace_modelconfig.root") ;
    RooWorkspace* w = (RooWorkspace*) f->Get("w") ;
    w->Print();

    //GET THE MODELCONFIG FOR S+B
    ModelConfig* mc = (ModelConfig*)w->obj("S+B_model");

    //fIXED params
    w->var("mean_bw")->setConstant(true);
    w->var("sigma_bw")->setConstant(true);

    //DEFINE POI
    RooRealVar* Ns = w->var("Ns");

    ProfileLikelihoodCalculator pl(*w->data("dataset"), *mc);
    pl.SetConfidenceLevel(0.68);
    LikelihoodInterval* interval = pl.GetInterval();

    // find the iterval on the first Parameter of Interest
    RooRealVar* firstPOI = (RooRealVar*) mc->GetParametersOfInterest()->first();
    double lowerLimit = interval->LowerLimit(*firstPOI);
    double upperLimit = interval->UpperLimit(*firstPOI);


    cout << "\n68% interval on " <<firstPOI->GetName()<<" is : ["<< lowerLimit << ", "<< upperLimit << "]" << endl;

    LikelihoodIntervalPlot* plPlot = new LikelihoodIntervalPlot(interval);


    // DRAW
    TCanvas* c = new TCanvas("c","ProfileLikelihood",800,600);
    plPlot->Draw();

    c->Print("profile_likelihood.pdf");


}
