#ifndef ROOTTOOLS_H
#define ROOTTOOLS_H

#include <TROOT.h>
#include <TString.h>

class TCanvas;
class TF1;
class TFile;
class TH1;
class TH2;
class TPaletteAxis;
class TVirtualPad;

namespace RootTools {
	// Export Images
	extern Bool_t gHasImgExportEnabled;

// 	extern TString gImgExportDir;

	extern Bool_t gImgExportPNG;
	extern Bool_t gImgExportEPS;
	extern Bool_t gImgExportPDF;

	void ExportPNG(TCanvas * can, const TString & path = "./");
	void ExportEPS(TCanvas * can, const TString & path = "./");
	void ExportPDF(TCanvas * can, const TString & path = "./");
	void ExportImages(TCanvas * can, const TString & path = "./");

	void SaveAndClose(TCanvas * can, TFile * f, Bool_t export_images = kTRUE, const TString & path = "./");

	// Progress Bar
	extern Int_t gBarPointWidth;
	extern Int_t gBarWidth;
	void ProgressBar(int num, int max);

	// Pt and Momentum functions
	Double_t MtY(Double_t * yP, Double_t * par);
	Double_t Momentum(Double_t * yP, Double_t * par);

	void DrawAngleLine(Double_t angle, Double_t xdraw=-10, Double_t ydraw=-10, Double_t angledraw=0);
	void DrawMomentumLine(Double_t mom, Double_t xdraw=-10, Double_t ydraw=-10, Double_t angledraw=0);

	void NicePad(TVirtualPad * pad, Float_t mT, Float_t mR, Float_t mB, Float_t mL);
	void NiceHistogram(TH1 * h, Int_t ndivx, Int_t ndivy, Float_t xls, Float_t xts, Float_t xto, Float_t yls, Float_t yts, Float_t yto, Bool_t centerY = kFALSE, Bool_t centerX = kFALSE);
	TPaletteAxis * NicePalette(TH2 * h, Float_t ls, Float_t ts=0, Float_t to=0);
	void AutoScale(TH1 * hdraw, TH1 * href);
	void AutoScaleF(TH1 * hdraw, TH1 * href);

	void AutoScale(TH1 * hdraw, TH1 * href1, TH1 * href2);

	Float_t calcTotalError(TH1 * h, Int_t bin_l, Int_t bin_u);

	TNamed * GetObjectFromFile(TFile * f, const TString & hname, const TString & suffix = "");
};

Double_t langaufun(Double_t *x, Double_t *par);
TF1 *langaufit(TH1 *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF);
Int_t langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM);
void langaus();
	
#endif /* ROOTTOOLS_H */