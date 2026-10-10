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

void discovery_standalone(){
 ///Create workspace
    RooWorkspace* w = new RooWorkspace("w");
    ///vars
    w->factory("x6[0,9]");

    ///////////////////ZACHET FUNCTION//////////////////////////
    //PARAM BREITWIGNER
    w->factory("mean_bw[3, 0.1, 10]");
    w->factory("sigma_bw[0.5, 0.1, 5]");

    //PARAM GAUS
    w->factory("mean_g[3, 0.1, 100]");
    w->factory("sigma_g[3, 2.5, 3.5]");

    w->factory("frac_b[560,0,700]");
    w->factory("frac_sg[140,0,700]");


    TFile* file = TFile::Open("treeForHypoTest.root");
    TTree* tree = (TTree*)file->Get("testtree");

    Float_t x_value;
    tree->SetBranchAddress("x6", &x_value);

    RooDataSet* ds = new RooDataSet("ds", "ds",
                                RooArgSet(*w->var("x6")));
    for (Long64_t i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        w->var("x6")->setVal(x_value);
        ds->add(RooArgSet(*w->var("x6")));
    }
    w->import(*ds);

    //print entries num
    std::cout << "dataset n = " << w->data("ds")->numEntries()<< std::endl;


    //FUNCTIONS !!!!
    w->factory("Gaussian::g(x6,mean_g,sigma_g)");
    w->factory("BreitWigner::bw(x6,mean_bw,sigma_bw)");

    //MODEL
    w->factory("SUM::model(frac_b*g, frac_sg*bw)");
    RooAbsPdf * model = w->pdf("model");

    //FIT MODEL TO DATA
    RooFitResult* r = model->fitTo(*w->data("ds"), Extended(kTRUE), Minimizer("Minuit2"),Save());

    w->defineSet("obs", "x6");
    w->defineSet("poi", "frac_sg");

    RooPlot* frame = w->var("x6")->frame();
    w->data("ds")->plotOn(frame);
    model->plotOn(frame, LineColor(kBlack));
    model->plotOn(frame, Components("bw"), LineColor(kRed), LineStyle(kDashed));
    model->plotOn(frame, Components("g"), LineColor(kBlue), LineStyle(kDashed));


    //ModelCOnfig null with s=0

    ModelConfig b_model("B_model", w);
    b_model.SetPdf(*w->pdf("model"));
    b_model.SetObservables(*w->set("obs"));
    b_model.SetParametersOfInterest(*w->set("poi"));
    w->var("frac_sg")->setVal(0.0);
    b_model.SetSnapshot(*w->set("poi"));

    //ModelCOnfig alternative with s=50

    ModelConfig sb_model("S+B_model", w);
    sb_model.SetPdf(*w->pdf("model"));
    sb_model.SetObservables(*w->set("obs"));
    sb_model.SetParametersOfInterest(*w->set("poi"));
    w->var("frac_sg")->setVal(50.0);
    sb_model.SetSnapshot(*w->set("poi"));

    w->defineSet("nuisParams", "mean_g,sigma_g,frac_b");
    b_model.SetNuisanceParameters(*w->set("nuisParams"));
    sb_model.SetNuisanceParameters(*w->set("nuisParams"));

    w->var("mean_bw")->setConstant(true);
    w->var("sigma_bw")->setConstant(true);

    ///IMPORT THE ModelConfig into WS
    w->import(sb_model);
    w->import(b_model);

     TH1* h1 = ds->createHistogram("x6");
    RooDataHist binData("obsData", "obsData", *w->var("x6"),h1 );
    w->import(binData);

    HypoTestCalculatorGeneric* hypoCalc = new AsymptoticCalculator(binData, sb_model, b_model);
    ((AsymptoticCalculator*) hypoCalc)->SetOneSidedDiscovery(true);
    HypoTestResult* htr = hypoCalc-> GetHypoTest();
    htr->Print();

    model->plotOn(frame, LineColor(kGreen), LineStyle(kDashed));

    TCanvas* c1 = new TCanvas("c1","fit",800,600);
    frame->Draw();



    c1->Print("discovery_standalone.pdf");




}
