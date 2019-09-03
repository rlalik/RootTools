#include <cppunit/extensions/HelperMacros.h>

#include <RootTools.h>
#include <TH1.h>

class BasicCase : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( BasicCase );
    CPPUNIT_TEST( MyTest );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {}

protected:
    void MyTest()
    {
//         float fnum = 2.00001f;
//         CPPUNIT_ASSERT_DOUBLES_EQUAL( fnum, 2.0f, 0.0005 );
//     
//         std::string pattern_string("%%d pattern");
//         std::string test_string("test pattern");
//         std::string replace_string("test");
//     
//         std::string output_string = SmartFactory::placeholder(pattern_string, "%%d", replace_string);
//         CPPUNIT_ASSERT_EQUAL(output_string, test_string);

        TH1D * h = new TH1D("h_errors_test", "h", 100, -5, 5);
        h->FillRandom("gaus", 1000);

        int bins = h->GetXaxis()->GetNbins();
        double err1 = RootTools::calcTotalError(h, 0, bins);
        double err2 = RootTools::calcTotalError2(h, 0, bins);

        printf(" err1 = %g\t err2 = %g\n", err1, err2);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( BasicCase );
