#ifndef ROOTTOOLS_H
#define ROOTTOOLS_H

#include <TCanvas.h>

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

	// Progress Bar
	extern Int_t gBarPointWidth;
	extern Int_t gBarWidth;
	void ProgressBar(int num, int max);
};

#endif /* ROOTTOOLS_H */