#ifndef ROOTTOOLS_H
#define ROOTTOOLS_H

#include <TH2.h>

class TCanvas;
class TF1;
class TFile;
class TGraph;
class TPad;
class TPaletteAxis;
class TVirtualPad;

#include <string>
#include <vector>

struct ErrorsPair
{
    double high;
    double low;
};

namespace RT
{

enum StatFlags
{
    SF_COUNTS = 0x01,
};

struct PadFormat
{
    Float_t marginTop;
    Float_t marginRight;
    Float_t marginBottom;
    Float_t marginLeft;
};

struct AxisFormat
{
    enum MODFLAGS
    {
        FNONE = 0,
        NDIV = 1,
        LS = 2,
        LO = 4,
        TS = 8,
        TO = 16,
        CL = 32,
        OPT = 64,
        FALL = 127
    };

    AxisFormat();
    AxisFormat(int ndiv, double ls, double lo, double ts, double to, bool center, bool opt,
               MODFLAGS flags = FALL);

    Int_t Ndiv;
    Float_t ls;
    Float_t lo;
    Float_t ts;
    Float_t to;
    Bool_t center_label;
    Bool_t optimize;

    int flags;
    void format(TAxis* ax) const;
};

struct GraphFormat
{
    AxisFormat x, y, z;
};

struct PaintFormat
{
    PadFormat pf;
    GraphFormat gf;
};

void def(PadFormat& f);
void def(GraphFormat& f);
void def(PaintFormat& f);

// Export Images
extern Bool_t gHasImgExportEnabled;

extern Bool_t gImgExportPNG;
extern Bool_t gImgExportEPS;
extern Bool_t gImgExportPDF;

void ExportPNG(TCanvas* can, const TString& path = "./");
void ExportEPS(TCanvas* can, const TString& path = "./");
void ExportPDF(TCanvas* can, const TString& path = "./");
void ExportMacroC(TCanvas* can, const TString& path = "./");
void ExportImages(TCanvas* can, const TString& path = "./");

void SaveAndClose(TCanvas* can, TFile* f, Bool_t export_images = kTRUE, const TString& path = "./");

// Pt and Momentum functions
Double_t MtY(Double_t* yP, Double_t* par);
Double_t Momentum(Double_t* yP, Double_t* par);

void DrawAngleLine(Double_t angle, Double_t xdraw = -10, Double_t ydraw = -10,
                   Double_t angledraw = 02, Int_t color = kBlack, Int_t width = 2, Int_t style = 2);
void DrawMomentumLine(Double_t mom, Double_t xdraw = -10, Double_t ydraw = -10,
                      Double_t angledraw = 02, Int_t color = kBlack, Int_t width = 2,
                      Int_t style = 2);
void DrawLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Int_t color = kBlack,
              Int_t width = 1, Int_t style = 1);

void NicePad(TVirtualPad* pad, Float_t mT, Float_t mR, Float_t mB, Float_t mL);
void NicePad(TVirtualPad* pad, const PadFormat& format);

void NiceHistogram(TH1* h, Int_t ndivx, Int_t ndivy, Float_t xls, Float_t xlo, Float_t xts,
                   Float_t xto, Float_t yls, Float_t ylo, Float_t yts, Float_t yto,
                   Bool_t centerX = kFALSE, Bool_t centerY = kFALSE, Bool_t optX = kTRUE,
                   Bool_t optY = kTRUE);
void NiceHistogram(TH1* h, const GraphFormat& format);
void NiceHistogram(TH1* h, const TString& text);

void NiceHistogram(TH2* h, const GraphFormat& format);

void NiceGraph(TGraph* gr, Int_t ndivx, Int_t ndivy, Float_t xls, Float_t xlo, Float_t xts,
               Float_t xto, Float_t yls, Float_t ylo, Float_t yts, Float_t yto,
               Bool_t centerX = kFALSE, Bool_t centerY = kFALSE, Bool_t optX = kTRUE,
               Bool_t optY = kTRUE);
void NiceGraph(TGraph* gr, const GraphFormat& format);

TPaletteAxis* NicePalette(TH2* h, Float_t ls, Float_t ts = 0, Float_t to = 0);
TPaletteAxis* NoPalette(TH2* h);

void AutoScale(TH1* hdraw, TH1* href, Bool_t MinOnZero = kTRUE);
void AutoScaleF(TH1* hdraw, TH1* href);
void AutoScale(TH1* hdraw, TH1* href1, TH1* href2);

std::pair<double, double> calcSubstractionError(TF1* total, TF1* bkg, double l, double u,
                                                bool verbose = false);
double calcTotalError(TH1* h, Int_t bin_l, Int_t bin_u);
double calcTotalError2(TH1* h, Int_t bin_l, Int_t bin_u);

TNamed* GetObjectFromFile(TFile* f, const TString& hname, const TString& suffix = "");

void NicePalette();

TH1* CloneHistSubrange(TH1* hist, char* name, Int_t bin_min, Int_t bin_max);
Int_t FindEqualIntegralRange(TH1* hist, Float_t integral, Int_t starting_bin, Int_t step,
                             Bool_t equal_or_bigger = kTRUE);

void QuickDraw(TVirtualPad* p, TH1* h, const char* opts = "", UChar_t logbits = 0);
void DrawStats(TVirtualPad* p, TH1* h, UInt_t flags = SF_COUNTS, Float_t x = 0.65, Float_t y = 0.85,
               Float_t dy = -0.05);

bool FindMaxRange(float& range, const TH1* hist);
bool FindMaxRange(float& range, float& cand);

void MyMath();
void FetchFitInfo(TF1* fun, double& mean, double& width, double& sig, double& bkg,
                  TPad* pad = nullptr);

bool Smooth(TH1* h);
bool Smooth(TH1* h, int loops);

float Normalize(TH1* h, TH1* href, bool extended = false);

TString MergeOptions(const TString& prefix, const TString& options, const TString& alt);

template <class T> void FindRangeExtremum(T& min, T& max, T& cand);
template <class T> void FindRangeExtremum(T& min, T& max, const TH1* hist);
template <class T> void FindRangeExtremum(T& min, T& max, const TH2* hist);

void FindBoundaries(TH1* h, Double_t& minimum, Double_t& maximum, Bool_t clean_run = kTRUE,
                    Bool_t with_error_bars = kTRUE);
void FindBoundaries(TGraph* h, Double_t& minimum, Double_t& maximum, Bool_t clean_run = kTRUE,
                    Bool_t with_error_bars = kTRUE);

bool FileIsNewer(const char* file, const char* reference = nullptr);

auto split(const std::string& s, char delim, std::vector<std::string>& elems)
    -> std::vector<std::string>&;
auto split(const std::string& s, char delim) -> std::vector<std::string>;

TF1* makeBarOffsetFunction(TF1* fun, double bar_width_scale);
double calcFuncErrorBar(TF1* fun, double x1, double x2, double bar_width_scale = 1.0,
                        int ccolor = 0);

void copyRelativeErrors(TH1* destination, TH1* source);
void calcBinomialErrors(TH1* p, TH1* N);
void calcBinomialErrors(TH1* p, TH1* q, TH1* N);

void calcErrorPropagationMult(TH1* h, double val, double err);
void calcErrorPropagationDiv(TH1* h, double val, double err);
auto errorsStrToArray(const std::string& errors_str) -> std::vector<ErrorsPair>;

double calcTotalContent(TH1* h, bool verbose = false);

double calcTotalError(TH1* h, bool verbose = false);
double calcTotalError(const std::vector<ErrorsPair>& errschain, double& err_u, double& err_l);

void calcTotalHistogramValues(TH1* h, double& content, double& error, bool verbose = false);

TH1* makeRelativeErrorHistogram(TH1* h, bool percentage = false);
}; // namespace RootTools

Double_t langaufun(Double_t* x, Double_t* par);
TF1* langaufit(TH1* his, Double_t* fitrange, Double_t* startvalues, Double_t* parlimitslo,
               Double_t* parlimitshi, Double_t* fitparams, Double_t* fiterrors, Double_t* ChiSqr,
               Int_t* NDF);
Int_t langaupro(Double_t* params, Double_t& maxx, Double_t& FWHM);
void langaus();

enum TermColors
{
    TC_Black,
    TC_Red,
    TC_Green,
    TC_Yellow,
    TC_Blue,
    TC_Purple,
    TC_Cyan,
    TC_White,
    TC_BlackB,
    TC_RedB,
    TC_GreenB,
    TC_YellowB,
    TC_BlueB,
    TC_PurpleB,
    TC_CyanB,
    TC_WhiteB,
    TC_None
};

std::ostream& colstd(std::ostream& str);
std::ostream& resstd(std::ostream& str);

struct smanip
{
    std::ostream& (*f)(std::ostream&, TermColors);
    TermColors i;
    smanip(std::ostream& (*ff)(std::ostream&, TermColors), TermColors ii) : f(ff), i(ii) {}
};

std::ostream& operator<<(std::ostream& os, const smanip& m);

std::ostream& set_color(std::ostream& s, TermColors c);
inline smanip color(TermColors n) { return smanip(set_color, n); }

template <class T> void RT::FindRangeExtremum(T& min, T& max, const TH1* hist)
{
    int max_rb = hist->GetMaximumBin();
    T max_r = hist->GetBinContent(max_rb);
    if (max_r > max) { max = max_r; }

    int min_rb = hist->GetMinimumBin();
    T min_r = hist->GetBinContent(min_rb);
    if (min_r < min) { min = min_r; }
}

template <class T> void RT::FindRangeExtremum(T& min, T& max, T& cand)
{
    if (cand > max) { max = cand; }

    if (cand < min) { min = cand; }
}

template <class T> void RT::FindRangeExtremum(T& min, T& max, const TH2* hist)
{
    int max_rb_x, max_rb_y, max_rb_z;
    hist->GetMaximumBin(max_rb_x, max_rb_y, max_rb_z);
    int min_rb_x, min_rb_y, min_rb_z;
    hist->GetMinimumBin(min_rb_x, min_rb_y, min_rb_z);

    T max_r = hist->GetBinContent(max_rb_x, max_rb_y);
    if (max_r > max) { max = max_r; }

    T min_r = hist->GetBinContent(min_rb_x, min_rb_y);
    if (min_r < min) { min = min_r; }
}

#endif /* ROOTTOOLS_H */
