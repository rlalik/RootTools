# ------------------------------------------------------------------------------
# gtest
check_3rd_party_tool(GTest 1.11.0 https://github.com/google/googletest TAG
                     release-1.11.0 AUTO)
# ------------------------------------------------------------------------------

find_package(GTest)

# configure_file(tests_config.h.in tests_config.h)

set(tests_SRCS tests_Basics.cpp tests_ProgressBar.cpp)

add_executable(roottools_tests ${tests_SRCS})

if(TARGET GTest::gtest_main)
  # new cmake provided
  set(GTEST_TRG GTest::gtest_main)
else()
  # old cmake and pc files
  set(GTEST_TRG gtest gtest_main)
endif()

target_link_libraries(roottools_tests PRIVATE RootTools ${GTEST_TRG})
# target_code_coverage(roottools_tests ALL)

gtest_discover_tests(roottools_tests)
