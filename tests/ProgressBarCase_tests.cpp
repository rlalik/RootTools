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
	RootTools::ProgressBar pb1(limit);
	pb1.setBarCharacter('x');
	for (int i = 0; i < limit; ++i)
		pb1++;
	pb1.close();

	RootTools::ProgressBar pb2(10000);
	pb2.setAlarmCharacter('+');
	pb2.setProgress(3000);
	pb2.setProgress(9000);
	pb2.setProgress(24000);
	pb2.close();
}
