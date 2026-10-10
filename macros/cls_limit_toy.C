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

void cls_limit_toy(){
 ///Create workspace
    RooWorkspace* w = new RooWorkspace("w");
    ///vars
    w->factory("x[-6,11]");

    ///////////////////ZACHET FUNCTION//////////////////////////
    //PARAM BREITWIGNER
    w->factory("mean_bw[-2.2, -3, 2]");
    w->factory("sigma_bw[1.3, 1.2, 2]");

    //PARAM GAUS
    w->factory("mean_g[3, 0.1, 100]");
    w->factory("sigma_g[0.5, 0.4, 0.5]");

        //FUNCTIONS !!!!
    w->factory("Gaussian::g(x,mean_g,sigma_g)");
    w->factory("Landau::bw(x,mean_bw,sigma_bw)");

    w->factory("sigfrac[10., 0., 100]");
    w->var("sigfrac")->setConstant(true);
    w->factory("bkgfrac[500, 0, 1000]");
    w->factory("SUM::model(sigfrac*bw, bkgfrac*g)");
    RooAbsPdf *model = w->pdf("model");

    RooRealVar *x = w->var("x");
    RooDataSet *ds = model->generate(*x, 500);
    w->import(*ds, Rename("ds"));
    std::cout << "dataset n = " << w->data("ds")->numEntries()<< std::endl;

    RooFitResult *r = model->fitTo(*ds, Minimizer("Minuit2"), Save());
    w->import(*r);

     w->defineSet("obs", "x");
    w->defineSet("poi", "sigfrac");

    ModelConfig b_model("B_model", w);
    b_model.SetPdf(*w->pdf("model"));
    b_model.SetObservables(*w->set("obs"));
    b_model.SetParametersOfInterest(*w->set("poi"));
    w->var("sigfrac")->setVal(0.0);
    b_model.SetSnapshot(*w->set("poi"));

    ModelConfig sb_model("SB_model", w);
    sb_model.SetPdf(*w->pdf("model"));
    sb_model.SetObservables(*w->set("obs"));
    sb_model.SetParametersOfInterest(*w->set("poi"));
    w->var("sigfrac")->setVal(10);
    sb_model.SetSnapshot(*w->set("poi"));

    w->defineSet("nuisParams", "mean_g,sigma_g,bkgfrac");
    b_model.SetNuisanceParameters(*w->set("nuisParams"));
    sb_model.SetNuisanceParameters(*w->set("nuisParams"));

    w->var("mean_bw")->setConstant(true);
    w->var("sigma_bw")->setConstant(true);

    w->import(b_model);
    w->import(sb_model);

    TH1 *h = ds->createHistogram("x");
    RooDataHist binData("obsData", "obsData", *w->var("x"), h);
    w->import(binData);

    RooAbsData *data = w->data("ds");

    HypoTestCalculatorGeneric *hypoCalc = new AsymptoticCalculator(binData, b_model, sb_model);
    ((AsymptoticCalculator*) hypoCalc)->SetOneSided(true);

    HypoTestInverter calc(*hypoCalc);
    calc.SetConfidenceLevel(0.95);
    calc.UseCLs(true);
    calc.SetVerbose(true);
    HypoTestInverterResult *res = calc.GetInterval();

    double upperLimit = res->UpperLimit();
    double ulError = res->UpperLimitEstimatedError();

    cout << "The computed upper limit is: " << upperLimit << " +/- " << ulError << endl;

    cout << "Expected upper limits, using the B (alternative) model: " << endl;

    for (int i=0; i<5; i++) {
        double sig = -2+i;
        cout << "Expected limit at " << sig << " sigma = " << res->GetExpectedUpperLimit(sig) << endl;
    }



}
