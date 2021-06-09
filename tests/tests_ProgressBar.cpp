#include <gtest/gtest.h>

#include <ProgressBar.h>

TEST(tests_ProgressBar, bar_test)
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
};
