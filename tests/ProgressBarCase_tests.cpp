#include <cppunit/extensions/HelperMacros.h>

#include <ProgressBar.h>
#include <TH1.h>

class ProgressBarCase_tests : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( class ProgressBarCase_tests );
	CPPUNIT_TEST( MyTest );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();

protected:
	void MyTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION( ProgressBarCase_tests );

void ProgressBarCase_tests::setUp()
{
}

void ProgressBarCase_tests::MyTest()
{
	const long limit = 100000;
	RootTools::ProgressBar pb(limit);
	
	for (int i = 0; i < limit; ++i)
		pb++;
}
