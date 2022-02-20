/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EXTRADIMENSIONMAPPER_H
#define EXTRADIMENSIONMAPPER_H

#include "DistributionContext.h"
#include "SmartFactory.h"

#ifdef HAVE_HISTASYMMERRORS
#include "TH2DA.h"
#endif

class TCanvas;
class TVirtualPad;

class ExtraDimensionMapper : public TObject, public SmartFactory
{
public:
    ExtraDimensionMapper(Dimensions dim, const std::string& name, TH1* hist, const AxisCfg& axis,
                         const std::string& dir_and_name);
    ExtraDimensionMapper(Dimensions dim, const std::string& name, TH1* hist, const AxisCfg& axis,
                         const std::string& dir_and_name, SmartFactory* sf);
    virtual ~ExtraDimensionMapper();

    Int_t getBinsX() const { return nbins_x; }
    Int_t getBinsY() const { return nbins_y; }
    Int_t getBinsZ() const { return nbins_z; }

    Int_t getBin(Int_t x, Int_t y = 0, Int_t z = 0) const;
    bool reverseBin(Int_t bin, Int_t& x) const;
    bool reverseBin(Int_t bin, Int_t& x, Int_t& y) const;
    bool reverseBin(Int_t bin, Int_t& x, Int_t& y, Int_t& z) const;

    TH1D* get(Int_t x, Int_t y = 0, Int_t z = 0);
    TH1D* find(Double_t x, Double_t y = 0.0, Double_t z = 0.0);
    TCanvas* getCanvas(Int_t x, Int_t y = 0);
    TVirtualPad* getPad(Int_t x, Int_t y = 0, Int_t z = 0);

    constexpr auto getNHists() const -> Int_t { return nhists; }
    TH1* operator[](int n) { return histograms[n]; }
    const TH1* operator[](int n) const { return histograms[n]; }

    void Fill1D(Double_t x, Double_t v, Double_t w = 1.0);
    void Fill2D(Double_t x, Double_t y, Double_t v, Double_t w = 1.0);
    void Fill3D(Double_t x, Double_t y, Double_t z, Double_t v, Double_t w = 1.0);

    // 	ExtraDimensionMapper & operator=(const ExtraDimensionMapper & fa);
private:
    void map1D(const AxisCfg& axis);
    void map2D(const AxisCfg& axis);
    void map3D(const AxisCfg& axis);
    void formatName(char* buff, Int_t x, Int_t y = 0, Int_t z = 0);
    void formatCanvasName(char* buff, Int_t x, Int_t y = 0);

public:
    Dimensions dim;
    AxisCfg axis; //||
    std::string prefix_name;

    Int_t nhists;
    Int_t nbins_x, nbins_y, nbins_z;

    TH1* ref_hist;
    TH1D** histograms;  //!
    TCanvas** canvases; //!

    // 	TObjArray * objectsFits;

private:
    // 	FitCallback * fitCallback;
};

#endif // EXTRADIMENSIONMAPPER_H
