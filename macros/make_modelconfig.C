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

void make_modelconfig(){
    //READ WORKSPACE
    TFile *f = new TFile("workspace_extended_fit.root") ;
    RooWorkspace* w = (RooWorkspace*) f->Get("w") ;

    //
    w->defineSet("obs", "x");
    w->defineSet("poi", "Ns");

    //ModelCOnfig null with s=0

    ModelConfig b_model("B_model", w);
    b_model.SetPdf(*w->pdf("model"));
    b_model.SetObservables(*w->set("obs"));
    b_model.SetParametersOfInterest(*w->set("poi"));
    w->var("Ns")->setVal(0.0);
    b_model.SetSnapshot(*w->set("poi"));

    //ModelCOnfig alternative with s=50

    ModelConfig sb_model("S+B_model", w);
    sb_model.SetPdf(*w->pdf("model"));
    sb_model.SetObservables(*w->set("obs"));
    sb_model.SetParametersOfInterest(*w->set("poi"));
    w->var("Ns")->setVal(50.0);
    sb_model.SetSnapshot(*w->set("poi"));

    w->defineSet("nuisParams", "mean_g,sigma_g,Nb");
    b_model.SetNuisanceParameters(*w->set("nuisParams"));
    sb_model.SetNuisanceParameters(*w->set("nuisParams"));

    w->var("mean_bw")->setConstant(true);
    w->var("sigma_bw")->setConstant(true);
    //w->var("mean_g")->SetConstant(true);
    //w->var("sigma_g")->SetConstant(true);

    ///IMPORT THE ModelConfig into WS
    w->import(sb_model);
    w->import(b_model);

    w->writeToFile("workspace_modelconfig.root");
    //w->Print();

}
