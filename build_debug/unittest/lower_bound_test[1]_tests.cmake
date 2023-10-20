add_test( lower_bound.test_lower_bound [==[/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/bin/lower_bound_test]==] [==[--gtest_filter=lower_bound.test_lower_bound]==] --gtest_also_run_disabled_tests)
set_tests_properties( lower_bound.test_lower_bound PROPERTIES WORKING_DIRECTORY [==[/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/unittest]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( lower_bound_test_TESTS lower_bound.test_lower_bound)
