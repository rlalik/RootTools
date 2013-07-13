#include "RootTools.h"

#include <TError.h>
#include <TASImage.h>
#include <TLatex.h>
#include <TMath.h>

#include <TCanvas.h>
#include <TColor.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TPaletteAxis.h>
#include <TStyle.h>
#include <TVirtualPad.h>

#include <iostream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace RootTools;

void RootTools::def(RootTools::PadFormat & f)
{
	f.marginTop			= 0.1;
	f.marginRight		= 0.1;
	f.marginBottom		= 0.1;
	f.marginLeft		= 0.1;
}

void RootTools::def(RootTools::GraphFormat & f)
{
	f.NdivX			= 505;
	f.NdivY			= 505;
	f.Xls			= 0.08;
	f.Xlo			= 0.005;
	f.Xts			= 0.08;
	f.Xto			= 0.9;
	f.Yls			= 0.08;
	f.Ylo			= 0.005;
	f.Yts			= 0.08;
	f.Yto			= 0.55;
	f.centerX		= kFALSE;
	f.centerY		= kFALSE;
	f.optX			= kTRUE;
	f.optY			= kTRUE;
}

void RootTools::def(RootTools::PaintFormat & f)
{
	PadFormat pf;
	def(pf);
	f.marginTop			= pf.marginTop;
	f.marginRight		= pf.marginRight;
	f.marginBottom		= pf.marginBottom;
	f.marginLeft		= pf.marginLeft;

	GraphFormat gf;
	def(gf);

	f.NdivX			= gf.NdivX;
	f.NdivY			= gf.NdivY;
	f.Xls			= gf.Xls;
	f.Xlo			= gf.Xlo;
	f.Xts			= gf.Xts;
	f.Xto			= gf.Xto;
	f.Yls			= gf.Yls;
	f.Ylo			= gf.Ylo;
	f.Yts			= gf.Yts;
	f.Yto			= gf.Yto;
	f.centerX		= gf.centerX;
	f.centerY		= gf.centerY;
	f.optX			= gf.optX;
	f.optY			= gf.optY;
}

PadFormat RootTools::fpad(PaintFormat f)
{
	return { f.marginTop, f.marginRight, f.marginBottom, f.marginLeft };
}

GraphFormat RootTools::fgraph(PaintFormat f)
{
	return {
		f.NdivX, f.NdivY,
		f.Xls, f.Xlo, f.Xts, f.Xto,
		f.Yls, f.Ylo, f.Yts, f.Yto,
		f.centerX, f.centerY, f.optX, f.optY
	};
}

Bool_t RootTools::gHasImgExportEnabled = kTRUE;

// TString RootTools::gImgExportDir = ".";

Bool_t RootTools::gImgExportPNG = kTRUE;
Bool_t RootTools::gImgExportEPS = kTRUE;
Bool_t RootTools::gImgExportPDF = kFALSE;

Int_t RootTools::gBarPointWidth = 50000;
Int_t RootTools::gBarWidth = 20;

void RootTools::ExportPNG(TCanvas * can, const TString & path) {
	TASImage img;
	img.FromPad(can);
	TString filename = path + TString::Format("%s.png", can->GetName());
	img.WriteImage(filename);
}

void RootTools::ExportEPS(TCanvas * can, const TString & path) {
	Int_t oldLevel = gErrorIgnoreLevel;
	gErrorIgnoreLevel = kWarning;
	TString filename = path + TString::Format("%s.eps", can->GetName());
	can->Print(filename);
	gErrorIgnoreLevel = oldLevel;
}

void RootTools::ExportPDF(TCanvas * can, const TString & path) {
	Int_t oldLevel = gErrorIgnoreLevel;
	gErrorIgnoreLevel = kWarning;
	TString filename = path + TString::Format("%s.pdf", can->GetName());
	can->Print(filename);
	gErrorIgnoreLevel = oldLevel;
}

void RootTools::ExportImages(TCanvas * can, const TString & path) {
	if (!gHasImgExportEnabled)
		return;

	if (gImgExportPNG)	ExportPNG(can, path);
	if (gImgExportEPS)	ExportEPS(can, path);
	if (gImgExportPDF)	ExportPDF(can, path);
}

void RootTools::ProgressBar(int num, int max) {
	static Int_t bw = 0;
	static Int_t bp = 0;
	static Bool_t newBar = kFALSE;

	if (num == 0) {
		newBar = kTRUE;
		bp = gBarPointWidth;
		bw = gBarWidth;
	}

	if (newBar) {
		std::cout << "==> Processing data " << std::flush;
		newBar = kFALSE;
	}
	
	if (num != 0  and (num+1) % bp == 0)
		std::cout << "." << std::flush;
	
	if ((num != 0  and (num+1) % (bp*bw) == 0) or (num == (max-1))) {
		Float_t num_percent = 100.0*num/(max-1);
		std::cout << " " << num+1 << " (" << num_percent << "%) " << "\n" << std::flush;
		newBar = kTRUE;
	}
}

void RootTools::SaveAndClose(TCanvas* can, TFile* f, Bool_t export_images, const TString & path) {
	can->Update();

	if (f)
		can->Write();

	if (export_images)
		ExportImages(can, path);

	can->Close();
}

//Theta in ptvsRap, von der Chii
Double_t RootTools::MtY(Double_t* yP, Double_t* par) {
	// Angle lines
	Double_t M        = par[0];
	Double_t ThetaLab = par[1];
	Double_t Part_A = 0.0;
	Double_t pt = 0.0;
	Double_t mt = 0.0;
	ThetaLab = TMath::Pi()*ThetaLab/180.0;
	Double_t y = yP[0];

	Part_A = pow(1/(tan(ThetaLab)*sinh(y)),2.0)-1.0;

	if (Part_A >= 0.0) {
		pt = M*pow(Part_A,-0.5);
		mt = sqrt(pt*pt+M*M)-M;
	}
	else
		return 1000000.0;

	if(pt > 0.0 && pt < 10000.0) {
		return pt;
	} else {
		return 1000000.0;
	}

	return pt;
}

//For lines showing the momentum; done by Malte
Double_t RootTools::Momentum(Double_t* yP, Double_t* par) {
	// Momentum lines
	Double_t Mass     = par[0];
	Double_t Momentum = par[1];
	Double_t y = yP[0];
	Double_t Energy;
	Double_t pz;
	Double_t pt;

	Energy = sqrt((pow(Momentum, 2.0)+pow(Mass, 2.0)));
	pz = ((exp(2*y)-1)/(1+exp(2*y)))*Energy;
	pt = sqrt(pow(Momentum,2.0)-pow(pz,2.0));

	if (pt > 0.0 && pt < 10000.0) {
		return pt;
	} else {
		return 0.0;
	}

	return pt;
}

void RootTools::DrawAngleLine(Double_t angle, Double_t xdraw, Double_t ydraw, Double_t angledraw) {
	static TF1* ThetaFunc = new TF1("ThetaFunc",&RootTools::MtY,-4,4,2);
	TString anglelabel = TString::Format("#theta=%2.0f#circ", angle);

	ThetaFunc->SetLineColor(2);

	ThetaFunc->FixParameter(0,1115.6);
	ThetaFunc->FixParameter(1,angle);
	ThetaFunc->SetLineWidth(1);
	ThetaFunc->SetLineStyle(2);
	ThetaFunc->DrawCopy("same c");

	TLatex* text = new TLatex;
	text->SetTextFont(42);
	text->SetTextSize(0.03);
	text->SetNDC();
	text->SetTextColor(1);
	text->SetTextAngle(angledraw);
	text->DrawLatex(xdraw,ydraw,anglelabel);
	delete text;
}

void RootTools::DrawMomentumLine(Double_t mom, Double_t xdraw, Double_t ydraw, Double_t angledraw) {
	static TF1 * PFunc = new TF1("PFunc", &RootTools::Momentum,-4,4,2);
	TString momentumlabel = TString::Format("p=%2.0f MeV/c", mom);

	PFunc->SetLineColor(2);

	PFunc->FixParameter(0,1115.6);
	PFunc->FixParameter(1,mom);
	PFunc->SetLineWidth(1);
	PFunc->SetLineStyle(2);
	PFunc->DrawCopy("same c");

	TLatex* text = new TLatex;
	text->SetTextFont(42);
	text->SetTextSize(0.03);
	text->SetNDC();
	text->SetTextColor(1);
	text->SetTextAngle(angledraw);
	//text->Draw();
	text->DrawLatex(xdraw, ydraw, momentumlabel);
	delete text;
}

void RootTools::DrawLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Int_t color, Int_t width, Int_t style)
{
	TLine * line = new TLine;
	line->SetLineColor(color);
	line->SetLineWidth(width);
	line->SetLineStyle(style);

	line->DrawLine(x1, y1, x2, y2);
}

TPaletteAxis * RootTools::NicePalette(TH2 * h, Float_t ls, Float_t ts, Float_t to) {
	gPad->Update();

	TPaletteAxis * axis = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");

	if (!axis) return NULL;

	axis->SetLabelSize(ls);
// 	axis->SetTitleSize(ts);
// 	axis->SetTitleOffset(to);

	return axis;
}

void RootTools::NicePad(TVirtualPad * pad, Float_t mT, Float_t mR, Float_t mB, Float_t mL)
{
// 	pad->Update();
	pad->SetTopMargin(mT);
	pad->SetBottomMargin(mB);
	pad->SetLeftMargin(mL);
	pad->SetRightMargin(mR);
}

void RootTools::NicePad(TVirtualPad * pad, const PadFormat & format)
{
	RootTools::NicePad(pad, format.marginTop, format.marginRight, format.marginBottom, format.marginLeft);
}

void RootTools::NiceHistogram(TH1 * h, Int_t ndivx, Int_t ndivy,
	Float_t xls, Float_t xlo, Float_t xts, Float_t xto,
	Float_t yls, Float_t ylo, Float_t yts, Float_t yto,
	Bool_t centerY, Bool_t centerX, Bool_t optX, Bool_t optY)
{

	h->GetXaxis()->SetNdivisions(ndivx, optX);
	h->GetYaxis()->SetNdivisions(ndivy, optY);

	h->SetStats(0);

	h->SetLabelSize(xls, "X");
	h->SetLabelSize(yls, "Y");
	h->SetLabelOffset(xlo, "X");
	h->SetLabelOffset(ylo, "Y");

	h->SetTitleSize(xts, "X");
	h->SetTitleSize(yts, "Y");
	h->SetTitleOffset(xto, "X");
	h->SetTitleOffset(yto, "Y");

	if (centerX)
		h->GetXaxis()->CenterTitle(centerX);
	if (centerY)
		h->GetYaxis()->CenterTitle(centerY);
}

void RootTools::NiceHistogram(TH1 * h, const GraphFormat & format)
{
	RootTools::NiceHistogram(h, format.NdivX, format.NdivY,
		format.Xls, format.Xlo, format.Xts, format.Xto, format.Yls, format.Ylo, format.Yts, format.Yto,
		format.centerX, format.centerY, format.optX, format.optY);
}

void RootTools::AutoScale(TH1 * hdraw, TH1 * href, Bool_t MinOnZero) {
	Float_t idrawmax = hdraw->GetBinContent(hdraw->GetMaximumBin());
	Float_t irefmax = href->GetBinContent(href->GetMaximumBin());

	Float_t idrawmin = hdraw->GetBinContent(hdraw->GetMinimumBin());
	Float_t irefmin = href->GetBinContent(href->GetMinimumBin());

	Float_t scalemax = irefmax > idrawmax ? irefmax : idrawmax;
	Float_t scalemin = irefmin < idrawmin ? irefmin : idrawmin;

	Float_t delta = scalemax - scalemin;
	scalemax += delta/2.;
	if (MinOnZero)
		scalemin = 0.;
	else
		scalemin -= delta/10.;
// 	scalemax = scalemax < 0. ? scalemax * 1.1 : scalemax * 0.9;
// 	scalemin = scalemin < 0. ? scalemin * 1.1 : scalemin * 0.9;
	hdraw->GetYaxis()->SetRangeUser(scalemin, scalemax);
}

void RootTools::AutoScale(TH1 * hdraw, TH1 * href1, TH1 * href2) {
	if (!href1 and !href2) {
		return;
	}

	if (!href1 and href2) {
		RootTools::AutoScale(hdraw, href2);
		return;
	}

	if (href1 and !href2) {
		RootTools::AutoScale(hdraw, href1);
		return;
	}

	Float_t idrawmax = hdraw->GetBinContent(hdraw->GetMaximumBin());
	Float_t iref1max = href1->GetBinContent(href1->GetMaximumBin());
	Float_t iref2max = href2->GetBinContent(href2->GetMaximumBin());

	Float_t idrawmin = hdraw->GetBinContent(hdraw->GetMinimumBin());
	Float_t iref1min = href1->GetBinContent(href1->GetMinimumBin());
	Float_t iref2min = href2->GetBinContent(href2->GetMinimumBin());

	Float_t irefmax = iref1max > iref2max ? iref1max : iref2max;
	Float_t irefmin = iref1min < iref2min ? iref1min : iref2min;

	Float_t scalemax = irefmax > idrawmax ? irefmax : idrawmax;
	Float_t scalemin = irefmin < idrawmin ? irefmin : idrawmin;

	Float_t delta = scalemax - scalemin;
	scalemax += delta/10.;
	scalemin -= delta/10.;
// 	scalemax = scalemax < 0. ? scalemax * 1.1 : scalemax * 0.9;
// 	scalemin = scalemin < 0. ? scalemin * 1.1 : scalemin * 0.9;
	hdraw->GetYaxis()->SetRangeUser(scalemin, scalemax);
}

void RootTools::AutoScaleF(TH1 * hdraw, TH1 * href) {
	TF1 * fdraw	= (TF1*)hdraw->GetListOfFunctions()->At(0);
	TF1 * fref	= (TF1*)href->GetListOfFunctions()->At(0);

	Float_t idrawmax;
	Float_t irefmax;
	if (fdraw)
		idrawmax = fdraw->GetMaximum();
	else
 		idrawmax = hdraw->GetMaximum();
	if (fref)
		irefmax = fref->GetMaximum();
	else
		irefmax = href->GetMaximum();

	Float_t idrawmin = hdraw->GetMinimum();
	Float_t irefmin = href->GetMinimum();

	Float_t scalemax = irefmax > idrawmax ? irefmax : idrawmax;
	Float_t scalemin = irefmin > idrawmin ? irefmin : idrawmin;

	Float_t delta = scalemax - scalemin;
	scalemax += delta/10.;
	scalemin -= delta/10.;
// 	scalemax = scalemax < 0. ? scalemax * 1.1 : scalemax * 0.9;
// 	scalemin = scalemin < 0. ? scalemin * 1.1 : scalemin * 0.9;
	hdraw->GetYaxis()->SetRangeUser(scalemin, scalemax);
}

Float_t RootTools::calcTotalError(TH1 * h, Int_t bin_l, Int_t bin_u) {
	Double_t val = 0.0;
	Double_t val_;
	for (Int_t i = bin_l; i <= bin_u; ++i) {
		val_ = h->GetBinError(i);
		val += val_ * val_;
	}

	return TMath::Sqrt(val);
}

TNamed * RootTools::GetObjectFromFile(TFile * f, const TString & name, const TString & suffix) {
	TNamed * dest = nullptr;

	dest = (TNamed*)f->Get(name);
	if (!dest) {
		std::cerr << "Error taking " << name.Data() << " from file " << "..." << ". Exiting..." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!suffix.IsNull()) {
		dest->SetName(name + TString("_") + suffix);
	}

	return dest;
}
void RootTools::NicePalette() {
	const Int_t NRGBs = 5;
	const Int_t NCont = 255;
	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };

	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	gStyle->SetNumberContours(NCont);

	gStyle->SetOptStat(0);
}