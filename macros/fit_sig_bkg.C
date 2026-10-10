#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooBreitWigner.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"

using namespace RooFit ;

void fit_sig_bkg()
{

    TFile* file = TFile::Open("testtree.root");
    TTree* tree = (TTree*)file->Get("testtree");

    RooRealVar x("x", "x", 0, 9);
    RooDataSet* dataset = new RooDataSet("dataset", "dataset", RooArgSet(x));


    Float_t x_value;
    tree->SetBranchAddress("x6", &x_value);


    // GET DATA

    Long64_t nentries = tree->GetEntries();

    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        x.setVal(x_value);
        dataset->add(RooArgSet(x));
    }
        std::cout << "dataset n =" << dataset->numEntries() << std::endl;

    //MAKE FUNCTION
    //PARAM BREITWIGNER
    RooRealVar mean_bw("mean_bw", "Mean Breit-Wigner", 3, 0.1, 100);
    RooRealVar gamma("gamma", "Gamma Breit-Wigner", 0.55, 0.1, 20);
    RooRealVar frac("frac", "Fraction", 0.2, 0, 1);

    //PARAM GAUS
    RooRealVar mean_gauss("mean_gauss", "Mean Gaussian",3, 0.1, 100);
    RooRealVar sigma("sigma", "Sigma Gaussian", 3, 0.1, 20);

    RooBreitWigner breitwigner("breitwigner", "Breit-Wigner PDF", x, mean_bw, gamma);
    RooGaussian gaussian("gaussian", "Gaussian PDF", x, mean_gauss, sigma);

    RooAddPdf model("model", "BreitWigner + Gaussian", RooArgList(breitwigner, gaussian), RooArgList(frac));

    model.fitTo(*dataset) ;

    TCanvas* c1 = new TCanvas("c1", "Result", 800, 600);
    RooPlot* frame = x.frame(Title("BreitWigner + Gaussian"));

    dataset->plotOn(frame);
    model.plotOn(frame);

    model.plotOn(frame, Components("breitwigner"), LineStyle(kDashed), LineColor(kRed));
    model.plotOn(frame, Components("gaussian"), LineStyle(kDashed), LineColor(kGreen));

    RooArgSet* parameters = breitwigner.getParameters(RooArgSet(x));
    parameters->Print("V");

    breitwigner.paramOn(frame,dataset) ;

    frame->Draw();

    c1->SaveAs("fit_sig_bkg.pdf");
}
