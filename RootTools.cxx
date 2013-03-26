#include "RootTools.h"

#include <TError.h>
#include <TImage.h>
#include <TLatex.h>
#include <TMath.h>

#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TPaletteAxis.h>
#include <TVirtualPad.h>

#include <iostream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace RootTools;

Bool_t RootTools::gHasImgExportEnabled = kTRUE;

// TString RootTools::gImgExportDir = ".";

Bool_t RootTools::gImgExportPNG = kTRUE;
Bool_t RootTools::gImgExportEPS = kTRUE;
Bool_t RootTools::gImgExportPDF = kFALSE;

Int_t RootTools::gBarPointWidth = 50000;
Int_t RootTools::gBarWidth = 20;

void RootTools::ExportPNG(TCanvas * can, const TString & path) {
	TImage * img = TImage::Create();
	img->FromPad(can);
	TString filename = path + TString::Format("%s.png", can->GetName());
	img->WriteImage(filename);
	delete img;
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

TPaletteAxis * RootTools::NicePalette(TH2 * h, Float_t ls, Float_t ts, Float_t to) {
	gPad->Update();

	TPaletteAxis * axis = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");

	if (!axis) return NULL;

	axis->SetLabelSize(ls);
// 	axis->SetTitleSize(ts);
// 	axis->SetTitleOffset(to);

	return axis;
}


void RootTools::NicePad(TVirtualPad * pad, Float_t mT, Float_t mR, Float_t mB, Float_t mL) {
// 	pad->Update();
	pad->SetTopMargin(mT);
	pad->SetBottomMargin(mB);
	pad->SetLeftMargin(mL);
	pad->SetRightMargin(mR);
}

void RootTools::NiceHistogram(TH1 * h, Int_t ndivx, Int_t ndivy, Float_t xls, Float_t xts, Float_t xto, Float_t yls, Float_t yts, Float_t yto, Bool_t centerY, Bool_t centerX) {

	h->GetXaxis()->SetNdivisions(ndivx);
	h->GetYaxis()->SetNdivisions(ndivy);

	h->SetStats(0);

	h->SetLabelSize(xls, "X");
	h->SetLabelSize(yls, "Y");

	h->SetTitleSize(xts, "X");
	h->SetTitleSize(yts, "Y");
	h->SetTitleOffset(xto, "X");
	h->SetTitleOffset(yto, "Y");

	if (centerX)
		h->GetXaxis()->CenterTitle(centerX);
	if (centerY)
		h->GetYaxis()->CenterTitle(centerY);
}

void RootTools::AutoScale(TH1 * hdraw, TH1 * href) {
	Float_t idrawmax = hdraw->GetMaximum();
	Float_t irefmax = href->GetMaximum();

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
	f->GetObject(name, dest);
	if (!dest) {
		std::cerr << "Error taking " << name.Data() << " from file " << "..." << ". Exiting..." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!suffix.IsNull()) {
		dest->SetName(name + TString("_") + suffix);
	}

	return dest;
}