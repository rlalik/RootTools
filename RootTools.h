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

// struct PadFormat {
// 	Float_t marginTop;
// 	Float_t marginRight;
// 	Float_t marginBottom;
// 	Float_t marginLeft;
// 
// 	PadFormat() :
// 		marginTop(0.1), marginRight(0.1), marginBottom(0.1), marginLeft(0.1) {}
// };
// 
// struct GraphFormat {
// 	Int_t NdivX;
// 	Int_t NdivY;
// 	Float_t Xls;
// 	Float_t Xlo;
// 	Float_t Xts;
// 	Float_t Xto;
// 	Float_t Yls;
// 	Float_t Ylo;
// 	Float_t Yts;
// 	Float_t Yto;
// 	Bool_t centerX;
// 	Bool_t centerY;
// 
// 	GraphFormat() :
// 		NdivX(505), NdivY(505),
// 		Xls(0.07), Xlo(0.05), Xts(0.07), Xto(0.05),
// 		Yls(0.07), Ylo(0.05), Yts(0.07), Yto(0.05),
// 		centerX(kFALSE), centerY(kFALSE) {}
// };

// struct PadFormat {
// 	Float_t marginTop		= 0.1;
// 	Float_t marginRight		= 0.1;
// 	Float_t marginBottom	= 0.1;
// 	Float_t marginLeft		= 0.1;
// };
// 
// struct GraphFormat {
// 	Int_t NdivX			= 505;
// 	Int_t NdivY			= 505;
// 	Float_t Xls			= 0.07;
// 	Float_t Xlo			= 0.05;
// 	Float_t Xts			= 0.07;
// 	Float_t Xto			= 0.05;
// 	Float_t Yls			= 0.07;
// 	Float_t Ylo			= 0.05;
// 	Float_t Yts			= 0.07;
// 	Float_t Yto			= 0.05;
// 	Bool_t centerX		= kFALSE;
// 	Bool_t centerY		= kFALSE;
// };
// 
// struct PaintFormat : public PadFormat, public GraphFormat {
// };

namespace RootTools {

	struct PadFormat {
		Float_t marginTop;
		Float_t marginRight;
		Float_t marginBottom;
		Float_t marginLeft;
	};
	
	struct GraphFormat {
		Int_t NdivX;
		Int_t NdivY;
		Float_t Xls;
		Float_t Xlo;
		Float_t Xts;
		Float_t Xto;
		Float_t Yls;
		Float_t Ylo;
		Float_t Yts;
		Float_t Yto;
		Bool_t centerX;
		Bool_t centerY;
		Bool_t optX;
		Bool_t optY;
	};

	struct PaintFormat {
		Float_t marginTop;
		Float_t marginRight;
		Float_t marginBottom;
		Float_t marginLeft;

		Int_t NdivX;
		Int_t NdivY;
		Float_t Xls;
		Float_t Xlo;
		Float_t Xts;
		Float_t Xto;
		Float_t Yls;
		Float_t Ylo;
		Float_t Yts;
		Float_t Yto;
		Bool_t centerX;
		Bool_t centerY;
		Bool_t optX;
		Bool_t optY;
	};

	void def(PadFormat & f);
	void def(GraphFormat & f);
	void def(PaintFormat & f);

	PadFormat fpad(PaintFormat f);
	GraphFormat fgraph(PaintFormat f);

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
	void DrawLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Int_t color = kBlack, Int_t width = 1, Int_t style = 1);

	void NicePad(TVirtualPad * pad, Float_t mT, Float_t mR, Float_t mB, Float_t mL);
	void NicePad(TVirtualPad * pad, const PadFormat & format);

	void NiceHistogram(TH1 * h, Int_t ndivx, Int_t ndivy, Float_t xls, Float_t xlo, Float_t xts, Float_t xto, Float_t yls, Float_t ylo, Float_t yts, Float_t yto, Bool_t centerY = kFALSE, Bool_t centerX = kFALSE, Bool_t optX = kTRUE, Bool_t optY = kTRUE);
	void NiceHistogram(TH1 * h, const GraphFormat & format);

	TPaletteAxis * NicePalette(TH2 * h, Float_t ls, Float_t ts=0, Float_t to=0);
	void AutoScale(TH1 * hdraw, TH1 * href, Bool_t MinOnZero = kTRUE);
	void AutoScaleF(TH1 * hdraw, TH1 * href);

	void AutoScale(TH1 * hdraw, TH1 * href1, TH1 * href2);

	Float_t calcTotalError(TH1 * h, Int_t bin_l, Int_t bin_u);

	TNamed * GetObjectFromFile(TFile * f, const TString & hname, const TString & suffix = "");

	void NicePalette();
};

Double_t langaufun(Double_t *x, Double_t *par);
TF1 *langaufit(TH1 *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF);
Int_t langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM);
void langaus();
	
#endif /* ROOTTOOLS_H */