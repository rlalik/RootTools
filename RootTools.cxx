#include "RootTools.h"

#include <TError.h>
#include <TImage.h>
#include <TLatex.h>
#include <TMath.h>

#include <iostream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace RootTools;

Bool_t RootTools::gHasImgExportEnabled = kTRUE;

TString RootTools::gImgExportDir = ".";

Bool_t RootTools::gImgExportPNG = kTRUE;
Bool_t RootTools::gImgExportEPS = kTRUE;
Bool_t RootTools::gImgExportPDF = kFALSE;

Int_t RootTools::gBarPointWidth = 50000;
Int_t RootTools::gBarWidth = 20;

void RootTools::ExportPNG(TCanvas * can) {
	TImage * img = TImage::Create();
	img->FromPad(can);
	TString filename = gImgExportDir + TString::Format("/%s.png", can->GetName());
	img->WriteImage(filename);
	delete img;
}

void RootTools::ExportEPS(TCanvas * can) {
	Int_t oldLevel = gErrorIgnoreLevel;
	gErrorIgnoreLevel = kWarning;
	TString filename = gImgExportDir + TString::Format("/%s.eps", can->GetName());
	can->Print(filename);
	gErrorIgnoreLevel = oldLevel;
}

void RootTools::ExportPDF(TCanvas * can) {
	Int_t oldLevel = gErrorIgnoreLevel;
	gErrorIgnoreLevel = kWarning;
	TString filename = gImgExportDir + TString::Format("/%s.pdf", can->GetName());
	can->Print(filename);
	gErrorIgnoreLevel = oldLevel;
}

void RootTools::ExportImages(TCanvas * can) {
	if (!gHasImgExportEnabled)
		return;

	if (gImgExportPNG)	ExportPNG(can);
	if (gImgExportEPS)	ExportEPS(can);
	if (gImgExportPDF)	ExportPDF(can);
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

void RootTools::SaveAndClose(TCanvas* can, TFile* f, Bool_t export_images) {
	can->Update();

	if (f)
		can->Write();

	if (export_images)
		ExportImages(can);

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

	ThetaFunc->SetLineColor(1);

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
	TString momentumlabel = TString::Format("p= %2.0f MeV/c", mom);

	PFunc->SetLineColor(1);

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