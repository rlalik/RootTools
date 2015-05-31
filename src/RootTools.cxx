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
#include <TLatex.h>
#include <TAttMarker.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TPad.h>
#include <TPaletteAxis.h>
#include <TStyle.h>
#include <TVirtualPad.h>
#include <TROOT.h>

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

	(void)mt;
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

void RootTools::DrawAngleLine(Double_t angle, Double_t xdraw, Double_t ydraw, Double_t angledraw, Int_t color, Int_t width, Int_t style)
{
	static TF1* ThetaFunc = new TF1("ThetaFunc",&RootTools::MtY,-4,4,2);
	TString anglelabel = TString::Format("#theta=%2.0f#circ", angle);

	ThetaFunc->SetLineColor(color);

	ThetaFunc->FixParameter(0,1115.6);
	ThetaFunc->FixParameter(1,angle);
	ThetaFunc->SetLineWidth(width);
	ThetaFunc->SetLineStyle(style);
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

void RootTools::DrawMomentumLine(Double_t mom, Double_t xdraw, Double_t ydraw, Double_t angledraw, Int_t color, Int_t width, Int_t style)
{
	static TF1 * PFunc = new TF1("PFunc", &RootTools::Momentum,-4,4,2);
	TString momentumlabel = TString::Format("p=%2.0f MeV/c", mom);

	PFunc->SetLineColor(color);

	PFunc->FixParameter(0,1115.6);
	PFunc->FixParameter(1,mom);
	PFunc->SetLineWidth(width);
	PFunc->SetLineStyle(style);
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

TPaletteAxis * RootTools::NicePalette(TH2 * h, Float_t ls, Float_t ts, Float_t to)
{
	(void)ls;
	(void)ts;
	(void)to;
	gPad->Update();

	TPaletteAxis * axis = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");

	if (!axis) return NULL;

// 	axis->SetLabelSize(ls);
// 	axis->SetTitleSize(ts);
// 	axis->SetTitleOffset(to);

	return axis;
}

TPaletteAxis * RootTools::NoPalette(TH2 * h)
{
	gPad->Update();

	TPaletteAxis * axis = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");

	if (!axis) return NULL;

	axis->Delete();
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

void RootTools::NiceHistogram(TH1 * h, const TString & text)
{
	TObjArray * arr = text.Tokenize(";,");

	size_t arr_len = arr->GetEntries();

	for (uint i = 0; i < arr_len; ++i)
	{
// 		PR(i);
		TObjArray * entry = ((TObjString *)arr->At(i))->GetString().Tokenize(":=");
		if (entry->GetEntries() != 2)
			continue;

		TString key = ((TObjString *)entry->At(0))->GetString();
		TString val = ((TObjString *)entry->At(1))->GetString();
// 		PR(key.Data());
// 		PR(val.Data());

		if (key == "lc")
			h->SetLineColor(val.Atoi());
		else if (key == "lw")
			h->SetLineWidth(val.Atoi());
		else if (key == "lt")
			h->SetLineStyle(val.Atoi());

		else if (key == "mc")
			h->SetMarkerColor(val.Atoi());
		else if (key == "ms")
			h->SetMarkerSize(val.Atof());
		else if (key == "mt")
			h->SetMarkerStyle(val.Atoi());

		else if (key == "fc")
			h->SetFillColor(val.Atoi());
		else if (key == "ft")
			h->SetFillStyle(val.Atoi());

		else if (key == "ho")
			h->SetOption(val);

		else
			printf(" - Unknow arguments %s = %s, skipping them...\n", key.Data(), val.Data());
	}
// 	char cmdchar = ((TObjString *)arr->At(0))->String()[0];
// 	TString tmpstr;
// 	char testchar = 0;
// 	bool usecorr = true;
// 	switch (cmdchar)
// 	{
// 		case 'e':
// 			cdfdata->reffile = ((TObjString *)arr->At(1))->String();
// 			break;
// 		case 'c':
// 			if (((TObjString *)arr->At(1))->String() == "maxy")
// 			{
// 				cfg_maxy[ ((TObjString *)arr->At(2))->String().Atoi() ] = ((TObjString *)arr->At(3))->String().Atof();
// 
// 					
// 	h->GetXaxis()->SetNdivisions(ndivx, optX);
// 	h->GetYaxis()->SetNdivisions(ndivy, optY);
// 
// 	h->SetStats(0);
// 
// 	h->SetLabelSize(xls, "X");
// 	h->SetLabelSize(yls, "Y");
// 	h->SetLabelOffset(xlo, "X");
// 	h->SetLabelOffset(ylo, "Y");
// 
// 	h->SetTitleSize(xts, "X");
// 	h->SetTitleSize(yts, "Y");
// 	h->SetTitleOffset(xto, "X");
// 	h->SetTitleOffset(yto, "Y");
// 
// 	if (centerX)
// 		h->GetXaxis()->CenterTitle(centerX);
// 	if (centerY)
// 		h->GetYaxis()->CenterTitle(centerY);
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
		std::cerr << "Error taking " << name.Data() << " from file " << f->GetPath() << ". Exiting..." << std::endl;
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

TH1 * RootTools::CloneHistSubrange(TH1 * hist, char * name, Int_t bin_min, Int_t bin_max)
{
	TH1 * h = (TH1*)hist->Clone(name);
	h->SetBins(bin_max - bin_min, hist->GetBinLowEdge(bin_min), hist->GetBinLowEdge(bin_max));

	for (Int_t i = 0; i < bin_max - bin_min; ++i)
	{
		h->SetBinContent(1+i, hist->GetBinContent(bin_min + i));
		h->SetBinError(1+i, hist->GetBinError(bin_min + i));
	}

	return h;
}

Int_t RootTools::FindEqualIntegralRange(TH1* hist, Float_t integral, Int_t starting_bin, Int_t step, Bool_t equal_or_bigger)
{
// 	Float_t eq_int = 0;

	Int_t x_min = 0;
	Int_t x_max = hist->GetNbinsX();

	for (Int_t sec_edge = starting_bin + step; ; sec_edge += step)
	{
		if ( (step > 0 and sec_edge > x_max ) or (step < 0 and sec_edge < x_min ))
			return starting_bin;

		Float_t tmp_int = 0.;
		if (step < 0)
			tmp_int = hist->Integral(sec_edge, starting_bin);
		if (step > 0)
			tmp_int = hist->Integral(starting_bin, sec_edge);

		if (tmp_int > integral)
		{
			if (equal_or_bigger)
			{
				return sec_edge;
			}
			else
			{
				return sec_edge - step;
			}
		}
	}

	return starting_bin;
}

void RootTools::QuickDraw(TVirtualPad * p, TH1 * h, const char * opts, UChar_t logbits)
{
	p->cd();
	h->Draw(opts);

	if (logbits & 0x01) p->SetLogz();
	if (logbits & 0x02) p->SetLogx();
	if (logbits & 0x04) p->SetLogy();
}

void RootTools::DrawStats(TVirtualPad * p, TH1 * h, UInt_t flags, Float_t x, Float_t y, Float_t dy)
{
	p->cd();

	Float_t next_y = y;

	TLatex * lt = new TLatex;
	lt->SetNDC();
	lt->SetTextSize(0.04);

// 	lt->SetTextColor(38);
// 	lt->SetTextColor(46);

	if (flags & SF_COUNTS)
	{
		lt->DrawLatex(x, next_y, TString::Format("Counts: %.1f", h->Integral()));
		next_y += dy;
	}
}

bool RootTools::FindMaxRange(float & range, const TH1 * hist)
{
	int max_rb = hist->GetMaximumBin();
	float max_r = hist->GetBinContent(max_rb);
	if (max_r > range)
	{
		range = max_r;
		return true;
	}
	
	return false;
}

bool RootTools::FindMaxRange(float & range, float & cand)
{
	if (cand > range)
	{
		range = cand;
		return true;
	}
	
	return false;
}

bool RootTools::FindRangeExtremum(float & min, float & max, const TH1 * hist)
{
	int max_rb = hist->GetMaximumBin();
	float max_r = hist->GetBinContent(max_rb);
	if (max_r > max)
	{
		max = max_r;
// 		return true;
	}

	int min_rb = hist->GetMinimumBin();
	float min_r = hist->GetBinContent(min_rb);
	if (min_r < min)
	{
		min = min_r;
// 		return true;
	}

	return false;
}

bool RootTools::FindRangeExtremum(float & min, float & max, float & cand)
{
	if (cand > max)
	{
		max = cand;
// 		return true;
	}

	if (cand < min)
	{
		min = cand;
// 		return true;
	}

	return false;
}


void RootTools::MyMath()
{
	if (!gROOT->GetListOfFunctions()->FindObject("voigt"))
	{
		new TF1("voigt", "[0] * TMath::Voigt(x - [1], [2], [3], 4)", -1, 1);
// 		new TF1("ggaus", "[0] * TMath::Gaus(x - [1], [2]) + [3]*TMath::Gaus(x - [4], [5])", 1095, 1150);
// 		new TF1("ggaus", "[0] * TMath::Exp(-0.5*((x-[1])/[2])**2) + [3] * TMath::Exp(-0.5*((x-[1])/[4])**2)", -1, 1);
// 		new TF1("g2aus", "[0] * TMath::Exp(-0.5*((x-[1])/[2])**2) + [3] * TMath::Exp(-0.5*((x-[1])/[4])**2)", -1, 1);
	}

	if (!gROOT->GetListOfFunctions()->FindObject("ggaus"))
	{
		new TF1("ggaus", "[0] * TMath::Exp(-0.5*((x-[1])/[2])**2) + [3] * TMath::Exp(-0.5*((x-[1])/[5])**2)", -1, 1);
// 		new TF1("ggaus", "[0] * TMath::Voigt(x - [1], [2], [5], 4)", -1, 1);
	}

	if (!gROOT->GetListOfFunctions()->FindObject("daus"))
	{
		new TF1("daus", "[0] * TMath::Exp(-0.5*((x-[1])/[2])**2) + [3] * TMath::Exp(-0.5*((x-[1])/[4])**2)", -1, 1);
	}

	if (!gROOT->GetListOfFunctions()->FindObject("aexpo"))
	{
		new TF1("aexpo", "[0] * exp([1]*(x-[2]))", -1, 1);
	}
}

void RootTools::FetchFitInfo(TF1 * fun, float & mean, float & width, float & sig, float & bkg, TPad * pad)
{
	(void)sig;
	(void)bkg;
	(void)pad;

	const char * ftitle = fun->GetTitle();

	if (strcmp(ftitle, "gaus(0)+gaus(3)") == 0)
	{
		Float_t fLambdaFitA1 = fun->GetParameter(0);
		Float_t fLambdaFitM1 = fun->GetParameter(1);
		Float_t fLambdaFitS1 = fun->GetParameter(2);

		Float_t fLambdaFitA2 = fun->GetParameter(3);
		Float_t fLambdaFitM2 = fun->GetParameter(1);
		Float_t fLambdaFitS2 = fun->GetParameter(5);

		mean = (fLambdaFitA1*fLambdaFitS1*fLambdaFitM1 + fLambdaFitA2*fLambdaFitS2*fLambdaFitM2)/(fLambdaFitA1*fLambdaFitS1 + fLambdaFitA2*fLambdaFitS2);
		width = (fLambdaFitA1*fLambdaFitS1*fLambdaFitS1 + fLambdaFitA2*fLambdaFitS2*fLambdaFitS2)/(fLambdaFitA1*fLambdaFitS1 + fLambdaFitA2*fLambdaFitS2);

		TLatex * latex = new TLatex();
		latex->SetNDC();
		latex->SetTextSize(0.03);
		latex->DrawLatex(0.57, 0.81, TString::Format("Signal : %s", fun->GetExpFormula().Data()));
		latex->DrawLatex(0.60, 0.77, TString::Format("A=%g", fLambdaFitA1));
		latex->DrawLatex(0.60, 0.74, TString::Format("#mu=%g", fLambdaFitM1));
		latex->DrawLatex(0.60, 0.71, TString::Format("#sigma=%g", fLambdaFitS1));
		latex->DrawLatex(0.75, 0.77, TString::Format("A=%g", fLambdaFitA2));
		latex->DrawLatex(0.75, 0.74, TString::Format("#mu=%g", fLambdaFitM2));
		latex->DrawLatex(0.75, 0.71, TString::Format("#sigma=%g", fLambdaFitS2));

		latex->DrawLatex(0.60, 0.67, TString::Format("/#mu=%g", mean));
		latex->DrawLatex(0.60, 0.64, TString::Format("/#sigma=%g", width));
	}

	if (strcmp(ftitle, "ggaus") == 0)
	{
		Float_t fLambdaFitA1 = fun->GetParameter(0);
		Float_t fLambdaFitA2 = fun->GetParameter(3);

		Float_t fLambdaFitM = fun->GetParameter(1);

		Float_t fLambdaFitS1 = TMath::Abs(fun->GetParameter(2));
		Float_t fLambdaFitS2 = TMath::Abs(fun->GetParameter(5));

		mean = fLambdaFitM;
		/* from Wiki about Voigt profile */
		width = (fLambdaFitA1*fLambdaFitS1*fLambdaFitS1 + fLambdaFitA2*fLambdaFitS2*fLambdaFitS2)/(fLambdaFitA1*fLambdaFitS1 + fLambdaFitA2*fLambdaFitS2);

		TLatex * latex = new TLatex();
		latex->SetNDC();
		latex->SetTextSize(0.03);
		latex->DrawLatex(0.57, 0.81, TString::Format("Signal : %s", fun->GetTitle()));
		latex->DrawLatex(0.60, 0.77, TString::Format("A=%g", fLambdaFitA1));
		latex->DrawLatex(0.60, 0.74, TString::Format("#sigma=%g", fLambdaFitS1));
		latex->DrawLatex(0.75, 0.77, TString::Format("A=%g", fLambdaFitA2));
		latex->DrawLatex(0.75, 0.74, TString::Format("#sigma=%g", fLambdaFitS2));
		latex->DrawLatex(0.60, 0.64, TString::Format("#mu=%g", fLambdaFitM));
		latex->DrawLatex(0.75, 0.64, TString::Format("/#sigma=%g", width));
	}

	if (strcmp(ftitle, "voigt") == 0)
	{
		Float_t fLambdaFitA = fun->GetParameter(0);
		Float_t fLambdaFitM = fun->GetParameter(1);
		Float_t fLambdaFitSL = fun->GetParameter(2);
		Float_t fLambdaFitSG = fun->GetParameter(3);

		mean = fLambdaFitM;
		/* from Wiki about Voigt profile */
		width = 0.5346 * fLambdaFitSL + TMath::Sqrt( 0.2166 * fLambdaFitSL * fLambdaFitSL + fLambdaFitSG * fLambdaFitSG);

		TLatex * latex = new TLatex();
		latex->SetNDC();
		latex->SetTextSize(0.03);
		latex->DrawLatex(0.57, 0.81, TString::Format("Signal : %s", fun->GetTitle()));
		latex->DrawLatex(0.60, 0.77, TString::Format("A=%g", fLambdaFitA));
		latex->DrawLatex(0.60, 0.74, TString::Format("#mu=%g", mean));
		latex->DrawLatex(0.60, 0.71, TString::Format("#sigma_{L}=%g", fLambdaFitSL));
		latex->DrawLatex(0.75, 0.71, TString::Format("#sigma_{G}=%g", fLambdaFitSG));
		latex->DrawLatex(0.60, 0.68, TString::Format("FWHM=%g", width));
	}
}

// bool RootTools::Smooth(TH1 * h, int par)
// {
// // 	TH1 * htmp = h->Clone("_XXYYZZ_temporary");
// // 	h->Delete();
// 
// 	size_t nbins = h->GetNbinsX();
// 	size_t abins = nbins - 2;
// 
// 	PR(abins);
// 
// 	bool * mark_bins = new bool[nbins];
// 	float * bc = new float[nbins-2];
// 	float * fc = new float[nbins-2];
// 
// 	for (uint i = 1; i < nbins-1; ++i)
// 	{
// 		bc[i-1] = h->GetBinContent(i);
// 		fc[i-1] = bc[i-1];
// 	}
// 
// 	for (uint i = 0; i < abins; ++i)
// 	{
// 		PR(i);
// 		if (i < 1)
// 		{
// 			if (
// 				(bc[i] > bc[i+1] and bc[i+1] < bc[i+2]) or
// 				(bc[i] < bc[i+1] and bc[i+1] > bc[i+2])
// 			)
// 			{
// 				float sum = bc[i] + bc[i+1];
// 				float dif = bc[i] - bc[i+1];
// 
// // 				float avg = sum/2.0;
// 
// 				float part = dif * 0.1;
// 				fc[i] -= part;
// 				fc[i+1] += part;
// 			}
// 		}
// 		else if (i > abins-2)
// 		{
// 			if (
// 				(bc[i] > bc[i+1] and bc[i-1] < bc[i]) or
// 				(bc[i] < bc[i+1] and bc[i-1] > bc[i])
// 			)
// 			{
// 				float sum = bc[i] + bc[i+1];
// 				float dif = bc[i] - bc[i+1];
// 
// // 				float avg = sum/2.0;
// 
// 				float part = dif * 0.1;
// 				fc[i] -= part;
// 				fc[i+1] += part;
// 			}
// 		}
// 		else
// 		{
// 			if (
// 				(bc[i] > bc[i+1] and bc[i+1] < bc[i+2] and bc[i-1] < bc[i]) or
// 				(bc[i] < bc[i+1] and bc[i+1] > bc[i+2] and bc[i-1] > bc[i])
// 			)
// 			{
// 				float sum = bc[i] + bc[i+1];
// 				float dif = bc[i] - bc[i+1];
// 
// // 				float avg = sum/2.0;
// 
// 				float part = dif * 0.1;
// 				fc[i] -= part;
// 				fc[i+1] += part;
// 			}
// 		}
// 	}
// 
// 	for (uint i = 1; i < nbins-1; ++i)
// 	{
// 		printf("%03d   : %f  --  %f\n", i, bc[i-1], fc[i-1]);
// 		h->SetBinContent(i, fc[i-1]);
// 	}
// 
// 	return true;
// }

bool RootTools::Smooth(TH1 * h)
{
// 	TH1 * htmp = h->Clone("_XXYYZZ_temporary");
// 	h->Delete();

	float mod_ratio = 0.2;
	float total_integral = h->Integral();

	size_t nbins = h->GetNbinsX();

	bool * mark_bins = new bool[nbins];
	float * bc = new float[nbins];
	float * fc = new float[nbins];
// 	float * dd = new float[nbins];
// 	float bw = h->GetBinWidth(1);

// 	dd[nbins-1] = 0.;

	for (uint i = 0; i < nbins; ++i)
	{
		bc[i] = h->GetBinContent(1+i);
		fc[i] = bc[i];

// 		if (i > 0)
// 			dd[i-1] = (bc[i] - bc[i-1])/bw;
	}

	for (uint i = 0; i < nbins; ++i)
	{
		if (i == 0)
		{
			if (
				(bc[i] > bc[i+1]) or
				(bc[i] < bc[i+1])
			)
			{
				float dif_r = bc[i] - bc[i+1];

				fc[i] -= (dif_r) * mod_ratio * 0.5;
				fc[i+1] += dif_r * mod_ratio * 0.5;
			}
		}
		else if (i == (nbins-1))
		{
			if (
				(bc[i-1] < bc[i]) or
				(bc[i-1] > bc[i])
			)
			{
				float dif_l = bc[i] - bc[i-1];

				fc[i-1] += dif_l * mod_ratio * 0.5;
				fc[i] -= (dif_l) * mod_ratio * 0.5;
			}
		}
		else
		{
			if (
// 				(bc[i] > bc[i+1] and bc[i-1] < bc[i]) or
// 				(bc[i] < bc[i+1] and bc[i-1] > bc[i])
				(bc[i] > bc[i+1] and bc[i+1] < bc[i+2] and bc[i-1] < bc[i]) or
				(bc[i] < bc[i+1] and bc[i+1] > bc[i+2] and bc[i-1] > bc[i])

			)
			{
				float dif_l = bc[i] - bc[i-1];
				float dif_r = bc[i] - bc[i+1];

				fc[i-1] += dif_l * mod_ratio;
				fc[i] -= (dif_l + dif_r) * mod_ratio;
				fc[i+1] += dif_r * mod_ratio;
			}
		}
	}

	for (uint i = 0; i < nbins; ++i)
	{
// 		printf("%03d   : %f  --  %f  ->  %f\n", i, bc[i], fc[i], dd[i]);
		h->SetBinContent(1+i, fc[i]);
// 		h->SetBinContent(1+i, dd[i]);
	}

	Float_t new_total_integral = h->Integral();

// 	printf("scaling by %f", total_integral/new_total_integral);
	h->Scale(total_integral/new_total_integral);

	delete [] bc;
	delete [] fc;
// 	delete [] dd;

	delete [] mark_bins; mark_bins = 0;
	return true;
}

bool RootTools::Smooth(TH1 * h, int loops)
{
	for (int i = 0; i < loops; ++i)
		if (!Smooth(h)) return false;

	return true;
}

float RootTools::Normalize(TH1 * h, TH1 * href, bool extended)
{
	if (!extended)
	{
		Float_t integral_ref = href->Integral();
		Float_t integral_cur = h->Integral();

		h->Scale(integral_ref/integral_cur);
		return integral_ref/integral_cur;
	}
	else
	{
		TH1 * hc_mask = (TH1*)h->Clone("___XXX___hc_mask");
		TH1 * hr_mask = (TH1*)href->Clone("___XXX___hr_mask");

		hc_mask->Divide(h);
		hr_mask->Divide(href);

		TH1 * hc_temp = (TH1*)h->Clone("___XXX___hc_temp");
		TH1 * hr_temp = (TH1*)href->Clone("___XXX___hr_temp");

		hc_temp->Multiply(hr_mask);
		hr_temp->Multiply(hc_mask);

		float scale = Normalize(hc_temp, hr_temp);
		h->Scale( scale, 0 );

		hc_mask->Delete();
		hr_mask->Delete();
		hc_temp->Delete();
		hr_temp->Delete();

		return scale;
	}
	return 0.;
}

const char * termcolors[TC_None+1] = {
	"\x1b[0;30m", "\x1b[0;31m", "\x1b[0;32m", "\x1b[0;33m",
	"\x1b[0;34m", "\x1b[0;35m", "\x1b[0;36m", "\x1b[0;37m",
	
	"\x1b[1;30m", "\x1b[1;31m", "\x1b[1;32m", "\x1b[1;33m",
	"\x1b[1;34m", "\x1b[1;35m", "\x1b[1;36m", "\x1b[1;37m",
	
	"\x1b[33;0m"
};

std::ostream & colstd(std::ostream & os)
{
	os << termcolors[TC_YellowB] << std::flush;
	return os;
}

std::ostream & resstd(std::ostream & os)
{
	os << termcolors[TC_None] << std::flush;
	return os;
}

std::ostream & operator<<(std::ostream & os, const smanip & m)
{
	return m.f(os, m.i);
}

std::ostream & set_color(std::ostream & s, TermColors c)
{
	s << termcolors[c] << std::flush;
	return s;
}