#ifndef ROOTTOOLS_H
#define ROOTTOOLS_H

#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>

namespace RootTools {
	// Export Images
	extern Bool_t gHasImgExportEnabled;

	extern TString gImgExportDir;

	extern Bool_t gImgExportPNG;
	extern Bool_t gImgExportEPS;
	extern Bool_t gImgExportPDF;

	void ExportPNG(TCanvas * can);
	void ExportEPS(TCanvas * can);
	void ExportPDF(TCanvas * can);
	void ExportImages(TCanvas * can);

	void SaveAndClose(TCanvas * can, TFile * f, Bool_t export_images = kTRUE);

	// Progress Bar
	extern Int_t gBarPointWidth;
	extern Int_t gBarWidth;
	void ProgressBar(int num, int max);
};

Double_t langaufun(Double_t *x, Double_t *par);
TF1 *langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF);
Int_t langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM);
void langaus();
	
#endif /* ROOTTOOLS_H */