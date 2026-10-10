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

void fit_extended(){
    ///Create workspace
    RooWorkspace* w = new RooWorkspace("w");

    ///vars
    w->factory("x[0,9]");

    ///////////////////ZACHET FUNCTION//////////////////////////
    w->factory("n_events[15000.]");
    w->factory("Ns[3000,2900,3100]");
    w->factory("Nb[12000,11900,12100]");

    //PARAM BREITWIGNER
    w->factory("mean_bw[3, 0.1, 100]");
    w->factory("sigma_bw[0.55, 0.1, 5]");

    //PARAM GAUS
    w->factory("mean_g[3, 0.1, 100]");
    w->factory("sigma_g[3, 2.5, 3.5]");



    //OPEN TREE
    TFile* file = TFile::Open("testtree.root");
    TTree* tree = (TTree*)file->Get("testtree");

    Float_t x_value;
    tree->SetBranchAddress("x6", &x_value);

    // GET DATA

    RooDataSet* dataset = new RooDataSet("dataset", "dataset",
                                RooArgSet(*w->var("x")));

    for (Long64_t i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        w->var("x")->setVal(x_value);
        dataset->add(RooArgSet(*w->var("x")));
    }

    w->import(*dataset);

    //print entries num
    std::cout << "dataset n = " << w->data("dataset")->numEntries()<< std::endl;


    //FUNCTIONS !!!! In the RooFit factory, class names are case-sensitive.
    //                  Use Gaussian, not gauss!!!!!
    w->factory("Gaussian::g(x,mean_g,sigma_g)");
    w->factory("BreitWigner::bw(x,mean_bw,sigma_bw)");

    //MODEL
    w->factory("SUM::model(Nb*g, Ns*bw)");
    RooAbsPdf * model = w->pdf("model");

    //FIT MODEL TO DATA
    RooFitResult* r = model->fitTo(*w->data("dataset"), Extended(kTRUE), Minimizer("Minuit2"),Save(true));

    ///////////////////VISUALIZATION NOT FOR HOMEWORK///////////////////////////////////

    //DRAW AND PLOT save to file
    RooPlot* frame = w->var("x")->frame();

    w->data("dataset")->plotOn(frame);

    model->plotOn(frame, LineColor(kBlack));
    model->plotOn(frame, Components("bw"), LineColor(kRed), LineStyle(kDashed));
    model->plotOn(frame, Components("g"), LineColor(kBlue), LineStyle(kDashed));

    TCanvas* c1 = new TCanvas("c1","fit",800,600);
    frame->Draw();
    c1->Print("fit_extended.pdf");

    //////////////////////SAVE WORKSPACE/////////////////////
    w->writeToFile("workspace_extended_fit.root") ;
    gDirectory->Add(w) ;
}
