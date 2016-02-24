#include "RootTools.h"

#include <TError.h>
#include <TASImage.h>
#include <TLatex.h>
#include <TMath.h>

#include <TCanvas.h>
#include <TColor.h>
#include <TFile.h>

#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>

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
#include <TSystem.h>
#include <TVirtualPad.h>
#include <TROOT.h>

#include <iostream>
#include <sstream>
#include <string>

#include <sys/stat.h>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace RootTools;

AxisFormat::AxisFormat() :
	Ndiv(505), ls(0.08), lo(0.005), ts(0.08), to(1.0), center_label(false), optimize(true), flags(FALL)
{
}

AxisFormat::AxisFormat(int ndiv, double ls, double lo, double ts, double to, bool center, bool opt, MODFLAGS flags) :
	Ndiv(ndiv), ls(ls), lo(lo), ts(ts), to(to), center_label(center), optimize(opt), flags(flags)
{
}

void AxisFormat::format(TAxis * ax) const
{
	if (flags | NDIV)	ax->SetNdivisions(Ndiv, optimize);
	if (flags | LS)		ax->SetLabelSize(ls);
	if (flags | LO)		ax->SetLabelOffset(lo);
	if (flags | TS)		ax->SetTitleSize(ts);
	if (flags | TO)		ax->SetTitleOffset(to);

	if (flags | CL and center_label)
		ax->CenterTitle(center_label);
}

void RootTools::def(RootTools::PadFormat & f)
{
	f.marginTop			= 0.1;
	f.marginRight		= 0.1;
	f.marginBottom		= 0.1;
	f.marginLeft		= 0.1;
}

void RootTools::def(RootTools::GraphFormat & f)
{
	f.x.Ndiv			= 505;
	f.x.ls				= 0.08;
	f.x.lo				= 0.005;
	f.x.ts				= 0.08;
	f.x.to				= 0.9;
	f.x.center_label	= kFALSE;
	f.x.optimize		= kTRUE;
	f.x.flags			= AxisFormat::FALL;

	f.y.Ndiv			= 505;
	f.y.ls				= 0.08;
	f.y.lo				= 0.005;
	f.y.ts				= 0.08;
	f.y.to				= 0.55;
	f.y.center_label	= kFALSE;
	f.y.optimize		= kTRUE;
	f.y.flags			= AxisFormat::FALL;
}

void RootTools::def(RootTools::PaintFormat & f)
{
	PadFormat pf;
	def(pf);
	f.pf.marginTop			= pf.marginTop;
	f.pf.marginRight		= pf.marginRight;
	f.pf.marginBottom		= pf.marginBottom;
	f.pf.marginLeft			= pf.marginLeft;

	GraphFormat gf;
	def(gf);

	f.gf.x					= gf.x;
	f.gf.y					= gf.y;
	f.gf.z					= gf.z;
}

Bool_t RootTools::gHasImgExportEnabled = kTRUE;

// TString RootTools::gImgExportDir = ".";

Bool_t RootTools::gImgExportPNG = kTRUE;
Bool_t RootTools::gImgExportEPS = kTRUE;
Bool_t RootTools::gImgExportPDF = kFALSE;

void RootTools::ExportPNG(TCanvas * can, const TString & path)
{
	TASImage img;
	img.FromPad(can);
	TString filename = path + TString::Format("%s.png", can->GetName());
	img.WriteImage(filename);
}

void RootTools::ExportEPS(TCanvas * can, const TString & path)
{
	Int_t oldLevel = gErrorIgnoreLevel;
	gErrorIgnoreLevel = kWarning;
	TString filename = path + TString::Format("%s.eps", can->GetName());
	can->Print(filename);
	gErrorIgnoreLevel = oldLevel;
}

void RootTools::ExportPDF(TCanvas * can, const TString & path)
{
	Int_t oldLevel = gErrorIgnoreLevel;
	gErrorIgnoreLevel = kWarning;
	TString filename = path + TString::Format("%s.pdf", can->GetName());
	can->Print(filename);
	gErrorIgnoreLevel = oldLevel;
}

void RootTools::ExportImages(TCanvas * can, const TString & path)
{
	if (!gHasImgExportEnabled)
		return;

	if (gImgExportPNG)	ExportPNG(can, path);
	if (gImgExportEPS)	ExportEPS(can, path);
	if (gImgExportPDF)	ExportPDF(can, path);
}

void RootTools::SaveAndClose(TCanvas* can, TFile* f, Bool_t export_images, const TString & path)
{
	can->Update();

	if (f)
		can->Write();

	if (export_images)
		ExportImages(can, path);

	can->Close();
}

//Theta in ptvsRap, von der Chii
Double_t RootTools::MtY(Double_t* yP, Double_t* par)
{
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
Double_t RootTools::Momentum(Double_t* yP, Double_t* par)
{
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

	if (!axis) return nullptr;

	return axis;
}

TPaletteAxis * RootTools::NoPalette(TH2 * h)
{
	gPad->Update();

	TPaletteAxis * axis = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");

	if (!axis) return nullptr;

	(TPaletteAxis*)h->GetListOfFunctions()->Remove(axis);
	axis->Delete();
	return 0;
}

void RootTools::NicePad(TVirtualPad * pad, Float_t mT, Float_t mR, Float_t mB, Float_t mL)
{
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
	Bool_t centerX, Bool_t centerY, Bool_t optX, Bool_t optY)
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
	RootTools::NiceHistogram(h, format.x.Ndiv, format.y.Ndiv,
		format.x.ls, format.x.lo, format.x.ts, format.x.to, format.y.ls, format.y.lo, format.y.ts, format.y.to,
		format.x.center_label, format.y.center_label, format.x.optimize, format.y.optimize);
}

void RootTools::NiceHistogram(TH2 * h, const GraphFormat & format)
{
	format.x.format(h->GetXaxis());
	format.y.format(h->GetYaxis());
	format.z.format(h->GetZaxis());
}

void RootTools::NiceHistogram(TH1 * h, const TString & text)
{
	TObjArray * arr = text.Tokenize(";");

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
}

void RootTools::NiceGraph(TGraph * gr, Int_t ndivx, Int_t ndivy,
	Float_t xls, Float_t xlo, Float_t xts, Float_t xto,
	Float_t yls, Float_t ylo, Float_t yts, Float_t yto,
	Bool_t centerX, Bool_t centerY, Bool_t optX, Bool_t optY)
{
	gr->GetXaxis()->SetNdivisions(ndivx, optX);
	gr->GetYaxis()->SetNdivisions(ndivy, optY);

	gr->GetXaxis()->SetLabelSize(xls);
	gr->GetYaxis()->SetLabelSize(yls);
	gr->GetXaxis()->SetLabelOffset(xlo);
	gr->GetYaxis()->SetLabelOffset(ylo);

	gr->GetXaxis()->SetTitleSize(xts);
	gr->GetYaxis()->SetTitleSize(yts);
	gr->GetXaxis()->SetTitleOffset(xto);
	gr->GetYaxis()->SetTitleOffset(yto);

	if (centerX)
		gr->GetXaxis()->CenterTitle(centerX);
	if (centerY)
		gr->GetYaxis()->CenterTitle(centerY);
}

void RootTools::NiceGraph(TGraph * gr, const GraphFormat & format)
{
	format.x.format(gr->GetXaxis());
	format.y.format(gr->GetYaxis());
}

void RootTools::AutoScale(TH1 * hdraw, TH1 * href, Bool_t MinOnZero)
{
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

void RootTools::AutoScale(TH1 * hdraw, TH1 * href1, TH1 * href2)
{
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
// 	scalemin -= delta/10.;
// 	scalemax = scalemax < 0. ? scalemax * 1.1 : scalemax * 0.9;
// 	scalemin = scalemin < 0. ? scalemin * 1.1 : scalemin * 0.9;
	hdraw->GetYaxis()->SetRangeUser(scalemin, scalemax);
}

void RootTools::AutoScaleF(TH1 * hdraw, TH1 * href)
{
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

std::pair<double, double> RootTools::calcSubstractionError(TF1 * total, TF1 * bkg, double l, double u, bool verbose)
{
	double int_b = bkg->Integral(l, u);
	double int_t = total->Integral(l, u);
	double s_delta = int_t - int_b;
	double s_error = sqrt(int_t + int_b);

	if (verbose)
		printf(" b=%g+-%g, t=%g+-%g, d=%g+-%g, err=%g == %g\n",
			int_b, sqrt(int_b), int_t, sqrt(int_t),
			s_delta, sqrt(s_delta), s_error, s_error/s_delta * 100);

	return std::pair<double, double>(s_delta, s_error);
}

double RootTools::calcTotalError(TH1 * h, Int_t bin_l, Int_t bin_u)
{
	double val = 0.0;
	double val_;
	for (Int_t i = bin_l; i <= bin_u; ++i) {
		val_ = h->GetBinError(i);
		val += val_ * val_;
	}

	return TMath::Sqrt(val);
}

double RootTools::calcTotalError2(TH1 * h, Int_t bin_l, Int_t bin_u)
{
	double val = 0.0;
	double val_;
	for (Int_t i = bin_l; i <= bin_u; ++i) {
		val_ = h->GetBinContent(i);
			val += val_;
	}

	return TMath::Sqrt(val);
}

TNamed * RootTools::GetObjectFromFile(TFile * f, const TString & name, const TString & suffix)
{
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

void RootTools::NicePalette()
{
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

void RootTools::MyMath()
{
	gSystem->Load("libMathMore");

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

	if (!gROOT->GetListOfFunctions()->FindObject("s2gaus"))
	{
		new TF1("s2gaus",
				"[0] * ([4] / ( [2] * TMath::Sqrt(2.0 * TMath::Pi()) ) * TMath::Exp(-0.5*((x-[1])/[2])**2) + " \
				"(1.0 - [4]) / ( [5] * TMath::Sqrt(2.0 * TMath::Pi()) ) * TMath::Exp(-0.5*((x-[1])/[5])**2))"
				, -1, 1);
// 		new TF1("ggaus", "[0] * TMath::Voigt(x - [1], [2], [5], 4)", -1, 1);
	}

	if (!gROOT->GetListOfFunctions()->FindObject("dgaus"))
	{
		new TF1("dgaus", "[0] * TMath::Exp(-0.5*((x-[1])/[2])**2) + [3] * TMath::Exp(-0.5*((x-[1])/[4])**2)", -1, 1);
	}

	if (!gROOT->GetListOfFunctions()->FindObject("aexpo"))
	{
		new TF1("aexpo", "[0] * exp([1]*(x-[2]))", -1, 1);
	}

	if (!gROOT->GetListOfFunctions()->FindObject("angdist"))
	{
		TString fbody = "([0]*ROOT::Math::legendre([3],x) + [1]*ROOT::Math::legendre([4],x) + [2]*ROOT::Math::legendre([5],x))/[0]";
		TF1 * legpol = new TF1("angdist", fbody, -1, 1);
		legpol->SetParameter(3, 0);
		legpol->SetParameter(4, 2);
		legpol->SetParameter(5, 4);
	}

	if (!gROOT->GetListOfFunctions()->FindObject("csangdist"))
	{
		TString fbody = "([0]*ROOT::Math::legendre([3],x) + [1]*ROOT::Math::legendre([4],x) + [2]*ROOT::Math::legendre([5],x)) * [6] * 0.5";
		TF1 * cslegpol = new TF1("csangdist", fbody, -1, 1);
		cslegpol->SetParameter(3, 0);
		cslegpol->SetParameter(4, 2);
		cslegpol->SetParameter(5, 4);

		cslegpol->SetParameter(0, 1);
		cslegpol->SetParameter(1, 1);
		cslegpol->SetParameter(2, 1);
		cslegpol->SetParameter(6, 1);
	}
}

void RootTools::FetchFitInfo(TF1 * fun, double & mean, double & width, double & sig, double & bkg, TPad * pad)
{
	(void)sig;
	(void)bkg;
	(void)pad;

	const char * ftitle = fun->GetTitle();

	if (strcmp(ftitle, "gaus(0)+gaus(3)") == 0)
	{
		double fLambdaFitA1 = fun->GetParameter(0);
		double fLambdaFitM1 = fun->GetParameter(1);
		double fLambdaFitS1 = fun->GetParameter(2);

		double fLambdaFitA2 = fun->GetParameter(3);
		double fLambdaFitM2 = fun->GetParameter(1);
		double fLambdaFitS2 = fun->GetParameter(5);

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
		double fLambdaFitA1 = fun->GetParameter(0);
		double fLambdaFitA2 = fun->GetParameter(3);

		double fLambdaFitM = fun->GetParameter(1);

		double fLambdaFitS1 = TMath::Abs(fun->GetParameter(2));
		double fLambdaFitS2 = TMath::Abs(fun->GetParameter(5));

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

	if (strcmp(ftitle, "dgaus") == 0)
	{
		double fLambdaFitA1 = fun->GetParameter(0);
		double fLambdaFitA2 = fun->GetParameter(3);

		double fLambdaFitM = fun->GetParameter(1);

		double fLambdaFitS1 = TMath::Abs(fun->GetParameter(2));
		double fLambdaFitS2 = TMath::Abs(fun->GetParameter(4));

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
		double fLambdaFitA = fun->GetParameter(0);
		double fLambdaFitM = fun->GetParameter(1);
		double fLambdaFitSL = fun->GetParameter(2);
		double fLambdaFitSG = fun->GetParameter(3);

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

	double new_total_integral = h->Integral();

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
		double integral_ref = href->Integral();
		double integral_cur = h->Integral();

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

TString RootTools::MergeOptions(const TString & prefix, const TString & options, const TString & alt)
{
	TString res;
	if (options.Length())
	{
		res = (prefix + "," + options);
	}
	else
	{
		res = (prefix + "," + alt);
	}

	return res;
}


/**
 * @brief Searches for minimal/maximal boundaries of the histogram data which are smaller/bigger than initial values. 
 * Boundaries search includes errorbars and skips empty bins w/o error bars.
 * By default, minimal/maximal variables are set to extreme values. If clean_run is reset, then initial values are kept.
 * 
 * @param h histogram to scan
 * @param minimum minimal value stored here
 * @param maximum maximal value stored here
 * @param clean_run reinitialize initial values
 * @param with_error_bars consider error bars for boundaries
 * @return void
 */
void RootTools::FindBoundaries(TH1* h, Double_t & minimum, Double_t & maximum, Bool_t clean_run, Bool_t with_error_bars)
{
	Int_t binx, biny, binz;
	TAxis * fXaxis = h->GetXaxis();
	TAxis * fYaxis = h->GetYaxis();
	TAxis * fZaxis = h->GetZaxis();

	Int_t xfirst  = fXaxis->GetFirst();
	Int_t xlast   = fXaxis->GetLast();
	Int_t yfirst  = fYaxis->GetFirst();
	Int_t ylast   = fYaxis->GetLast();
	Int_t zfirst  = fZaxis->GetFirst();
	Int_t zlast   = fZaxis->GetLast();

	if (clean_run)
	{
		minimum = FLT_MAX;
		maximum = FLT_MIN;
	}

	for (binz = zfirst; binz <= zlast; ++binz)
		for (biny = yfirst; biny <= ylast; ++biny)
			for (binx = xfirst; binx <= xlast; ++binx)
			{
				Double_t bc = h->GetBinContent(binx, biny, binz);
				Double_t be = 0.0;
				if (with_error_bars)
					h->GetBinError(binx, biny, binz);
				if (bc != 0 || (!with_error_bars or be != 0.0))
				{
					if (bc + be > maximum)	maximum = bc + be;
					if (bc - be < minimum)	minimum = bc - be;
				}
			}
}

/**
 * @brief Searches for minimal/maximal boundaries of the graph which are smaller/bigger than initial values. 
 * Boundaries search includes assymetric errorbars.
 * By default, minimal/maximal variables are set to extreme values. If clean_run is reset, then initial values are kept.
 * 
 * @param gr graph to scan
 * @param minimum minimal value stored here
 * @param maximum maximal value stored here
 * @param clean_run reinitialize initial values
 * @param with_error_bars consider error bars for boundaries
 * @return void
 */
void RootTools::FindBoundaries(TGraph * gr, Double_t & minimum, Double_t & maximum, Bool_t clean_run, Bool_t with_error_bars)
{
	Int_t points = gr->GetN();	

	Double_t p_x, p_y;

	if (clean_run)
	{
		minimum = FLT_MAX;
		maximum = FLT_MIN;
	}

	for (int p = 0; p < points; ++p)
	{
		gr->GetPoint(p, p_x, p_y);
		Double_t greyl = 0.0;
		Double_t greyh = 0.0;
		if (with_error_bars)
		{
			gr->GetErrorYlow(p);
			gr->GetErrorYhigh(p);
		}

		if (p_y + greyh > maximum)	maximum = p_y + greyh;
		if (p_y - greyl < minimum)	minimum = p_y - greyl;
	}
}

bool RootTools::FileIsNewer(const char* file, const char* reference)
{
	struct stat st_ref;
	struct stat st_aux;

	long long int mod_ref = 0;
	long long int mod_aux = 0;

	if (stat(file, &st_aux))
	{
		perror(file);
		if (!reference)	return false;
	}
	else
	{
		if (!reference)	return true;
		mod_aux = (long long)st_aux.st_mtim.tv_sec;
	}

	if (stat(reference, &st_ref))
	{
		perror(reference);
	}
	else
	{
		mod_ref = (long long)st_ref.st_mtim.tv_sec;
	}

	return mod_aux > mod_ref;
}

StringsVector & RootTools::split(const std::string & s, char delim, StringsVector & elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

StringsVector RootTools::split(const std::string & s, char delim)
{
	StringsVector elems;
	split(s, delim, elems);
	return elems;
}

TF1 * RootTools::makeBarOffsetFunction(TF1 * fun, double bar_width_scale)
{
	TF1 * f = new TF1();
	fun->Copy(*f);

	const size_t npars = fun->GetNpar();
	double * pars = new double[npars];
	double * errs = new double[npars];

	// backup original params and errors
	for (unsigned int i = 0; i < npars; ++i)
	{
		pars[i] = fun->GetParameter(i);
		errs[i] = fun->GetParError(i);
	}

	// set higher params, integrate
	for (unsigned int i = 0; i < npars; ++i)
	{
		f->SetParameter(i, pars[i] + bar_width_scale * errs[i]);
		f->SetParError(i, errs[i]);
	}

	return f;
}

double RootTools::calcFuncErrorBar(TF1 * fun, double x1, double x2, double bar_width_scale, int /*ccolor*/)
{

	TF1 * f_l = makeBarOffsetFunction(fun, -bar_width_scale);
	TF1 * f_u = makeBarOffsetFunction(fun, +bar_width_scale);

	double int_u = f_u->Integral(x1, x2);
	double int_l = f_l->Integral(x1, x2);

	f_l->Delete();
	f_u->Delete();
	double delta = fabs(int_u - int_l);

// 	printf("integrals: l=%g, u=%g, d=%g\n", int_l, int_u, delta);
	// calculate error-band area

	return delta;
}

void RootTools::copyRelativeErrors(TH1* destination, TH1* source)
{
	Int_t binx, biny, binz;
	TAxis * fXaxis = destination->GetXaxis();
	TAxis * fYaxis = destination->GetYaxis();
	TAxis * fZaxis = destination->GetZaxis();

	Int_t xfirst  = fXaxis->GetFirst();
	Int_t xlast   = fXaxis->GetLast();
	Int_t yfirst  = fYaxis->GetFirst();
	Int_t ylast   = fYaxis->GetLast();
	Int_t zfirst  = fZaxis->GetFirst();
	Int_t zlast   = fZaxis->GetLast();

	for (binz = zfirst; binz <= zlast; ++binz)
		for (biny = yfirst; biny <= ylast; ++biny)
			for (binx = xfirst; binx <= xlast; ++binx)
			{
				Double_t bc = source->GetBinContent(binx, biny, binz);
				Double_t be = source->GetBinError(binx, biny, binz);

				if (be != 0)
				{
					double bc_dst = destination->GetBinContent(binx, biny, binz);
// 					double be_dst = destination->GetBinError(binx, biny, binz);
// 					PR(bc_dst);
// 					PR(be_dst);
// 	m				PR(bc);
// 					PR(be);
// 					PR(be/bc);
// 					PR(destination->GetBinError(binx, biny, binz));
					destination->SetBinError(binx, biny, binz, bc_dst * be/bc);
// 					PR(destination->GetBinError(binx, biny, binz));
				}
				else
				{
					destination->SetBinError(binx, biny, binz, 0.0);
				}
			}
}

void RootTools::calcBinomialErrors(TH1* p, TH1* N)
{
	size_t bins_x = p->GetXaxis()->GetNbins();
	size_t bins_y = p->GetYaxis()->GetNbins();

	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double _p = p->GetBinContent(x, y);
			double _n = N->GetBinContent(x, y);

			double sigma = sqrt(_p * (1.0 - _p) * _n);
			p->SetBinError(x, y, sigma/_n);
		}
	}
}

void RootTools::calcBinomialErrors(TH1* p, TH1* q, TH1* N)
{
	size_t bins_x = p->GetXaxis()->GetNbins();
	size_t bins_y = p->GetYaxis()->GetNbins();

	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double _p = p->GetBinContent(x, y);
			double _q = q->GetBinContent(x, y);
			double _n = N->GetBinContent(x, y);

			double sigma = sqrt(_p * _q * _n);
			p->SetBinError(x, y, sigma/_n);
		}
	}
}

void RootTools::calcErrorPropagationMult(TH1* h, double val, double err)
{
	size_t bins_x = h->GetXaxis()->GetNbins();
	size_t bins_y = h->GetYaxis()->GetNbins();

	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double bc = h->GetBinContent(x, y) / val;
			double be = h->GetBinError(x, y) / val;

			double sigma = sqrt(val * val * be * be + bc * bc * err * err);
			h->SetBinError(x, y, sigma);
		}
	}
}

void RootTools::calcErrorPropagationDiv(TH1* h, double val, double err)
{
	size_t bins_x = h->GetXaxis()->GetNbins();
	size_t bins_y = h->GetYaxis()->GetNbins();

	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double bc = h->GetBinContent(x, y) * val;
			double be = h->GetBinError(x, y) * val;

			double sigma = sqrt(val * val * be * be + bc * bc * err * err) / ( val * val );
			h->SetBinError(x, y, sigma);
		}
	}
}

ErrorsChain RootTools::errorsStrToArray(const std::string& errors_str)
{
	ErrorsChain errors;
	size_t global_pos = 0;
	size_t str_end = errors_str.npos;

	while (global_pos < str_end)
	{
		size_t current_pos = global_pos;
		size_t section_end = errors_str.find('|', global_pos);

		ErrorsPair ep = { 0.0, 0.0 };
		std::string str_section = errors_str.substr(current_pos, section_end-current_pos);

		size_t control_signs_start = 0;
		size_t control_signs_stop = 0;
		size_t str_section_end = str_section.npos;

		while (control_signs_stop != str_section_end)
		{
			control_signs_stop = str_section.find_first_of("+-", control_signs_start);

			if (control_signs_start != control_signs_stop)
			{
				std::string sub_section = str_section.substr(control_signs_start, control_signs_stop);

				if (control_signs_stop == str_section_end)
				{
					if (sub_section.length())
						ep.low = ep.high = std::stod(sub_section);
					break;
				}
				else
				{
					ep.low = ep.high = std::stod(sub_section);
// 					continue;
					break;
				}
			}

			if (control_signs_start == control_signs_stop)
			{
				control_signs_stop = str_section.find_first_of("+-", control_signs_start+1);

				if (str_section[control_signs_start] == '+')
				{
					std::string sub_section = str_section.substr(control_signs_start+1, control_signs_stop == str_section_end ? str_section_end : control_signs_stop - 2 - control_signs_start + 1);
					ep.high = std::stod(sub_section);
				}
				if (str_section[control_signs_start] == '-')
				{
					std::string sub_section = str_section.substr(control_signs_start+1, control_signs_stop == str_section_end ? str_section_end : control_signs_stop - 2 - control_signs_start + 1);
					ep.low = std::stod(sub_section);
				}
			}

			control_signs_start = control_signs_stop;
		}
		errors.push_back(ep);
		global_pos = section_end+1;

// 		printf(" found errors pair: low=%g    high=%g    from %s\n", ep.low, ep.high, errors_str.c_str());

		if (section_end == str_end)
			break;
	}

	return errors;
}

double RootTools::calcTotalError(const ErrorsChain& errschain, double& err_u, double& err_l)
{
	err_u = 0;
	err_l = 0;
	for (uint i = 0; i < errschain.size(); ++i)
	{
		err_u += errschain[i].high * errschain[i].high;
		err_l += errschain[i].low * errschain[i].low;
	}

	double err = sqrt(err_u + err_l);

	err_u = sqrt(err_u);
	err_l = sqrt(err_l);

	return err;
}

double RootTools::calcTotalError(TH1* h, bool verbose)
{
	size_t bins_x = h->GetXaxis()->GetNbins();
	size_t bins_y = h->GetYaxis()->GetNbins();

	double total_err = 0.0;
	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double err = h->GetBinError(x, y);
			if (err != 0.0)
			{
				total_err += err*err;
				if (verbose)
					printf("[%lu, %lu] Adding %g * %g = %g -> %g\n", x, y, err, err, err*err, total_err);
			}
		}
	}

	printf("  sqrt(%g) = %g\n", total_err, sqrt(total_err));
	return sqrt(total_err);
}

double RootTools::calcTotalContent(TH1* h, bool verbose)
{
	size_t bins_x = h->GetXaxis()->GetNbins();
	size_t bins_y = h->GetYaxis()->GetNbins();

	double total_content = 0.0;
	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double cont = h->GetBinContent(x, y);
			if (cont != 0.0)
			{
				total_content += cont;
				if (verbose)
					printf("[%lu, %lu] Adding %g -> %g\n", x, y, cont, total_content);
			}
		}
	}

	printf("  content = %g\n", total_content);
	return total_content;
}

void RootTools::calcTotalHistogramValues(TH1* h, double & content, double & error, bool verbose)
{
	size_t bins_x = h->GetXaxis()->GetNbins();
	size_t bins_y = h->GetYaxis()->GetNbins();

	double total_content = 0.0;
	double total_err = 0.0;
	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double cont = h->GetBinContent(x, y);
			double err = h->GetBinError(x, y);

			total_content += cont;
			total_err += err*err;
			if (verbose)
			{
				printf("[%lu, %lu] V: %g -> %g  E: %g * %g = %g -> %g\n", x, y, cont, total_content, err, err, err*err, total_err);
			}
		}
	}

	content = total_content;
	error = sqrt(total_err);

	printf("  content = %g  sqrt(%g) = %g\n", content, total_err, error);
}

TH1 * RootTools::makeRelativeErrorHistogram(TH1* h, bool percentage)
{
	TH1 * re = (TH1*)h->Clone();
	re->Reset();

	size_t bins_x = h->GetXaxis()->GetNbins();
	size_t bins_y = h->GetYaxis()->GetNbins();

	for (size_t x = 1; x <= bins_x; ++x)
	{
		for (size_t y = 1; y <= bins_y; ++y)
		{
			double cont = h->GetBinContent(x, y);
			double err = h->GetBinError(x, y);

			if (percentage)
				re->SetBinContent(x, y, err/cont * 100.0);
			else
				re->SetBinContent(x, y, err/cont);
		}
	}

	return re;
}
