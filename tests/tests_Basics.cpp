#include <gtest/gtest.h>

#include <RootTools.h>
#include <TH1.h>

TEST(tests_Basics, errors_test)
{
    TH1D* h = new TH1D("h_errors_test", "h", 100, -5, 5);
    h->FillRandom("gaus", 1000);

    int bins = h->GetXaxis()->GetNbins();
    double err1 = RootTools::calcTotalError(h, 0, bins);
    double err2 = RootTools::calcTotalError2(h, 0, bins);

    printf(" err1 = %g\t err2 = %g\n", err1, err2);
};
