#include "RootTools.h"
#include <TImage.h>
#include <TError.h>

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