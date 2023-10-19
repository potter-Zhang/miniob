add_test( test_persist.test_persist_file_io [==[/home/someone/桌面/myd/miniob合并/sdk_miniob/build/bin/persist_test]==] [==[--gtest_filter=test_persist.test_persist_file_io]==] --gtest_also_run_disabled_tests)
set_tests_properties( test_persist.test_persist_file_io PROPERTIES WORKING_DIRECTORY [==[/home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( persist_test_TESTS test_persist.test_persist_file_io)
