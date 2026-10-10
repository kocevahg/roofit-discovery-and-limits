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

void discovery_from_workspace()
    {

    TFile *f = TFile::Open("workspace_modelconfig.root");
    RooWorkspace* w = (RooWorkspace*) f->Get("w");

    // GET SB AND B MODEL

    ModelConfig* sb_model = (ModelConfig*) w->obj("S+B_model");
    ModelConfig* b_model  = (ModelConfig*) w->obj("B_model");

    // FIX PARAM FOR NULL
    w->var("mean_bw")->setConstant(true);
    w->var("sigma_bw")->setConstant(true);

    RooAbsData *data = w->data("dataset");

    AsymptoticCalculator hypoCalc(*data, *sb_model, *b_model);
    hypoCalc.SetOneSidedDiscovery(true);

    HypoTestResult* htr = hypoCalc.GetHypoTest();
    htr->Print();



}
