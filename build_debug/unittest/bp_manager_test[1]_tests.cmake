add_test( test_frame_manager.test_frame_manager_simple_lru [==[/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/bin/bp_manager_test]==] [==[--gtest_filter=test_frame_manager.test_frame_manager_simple_lru]==] --gtest_also_run_disabled_tests)
set_tests_properties( test_frame_manager.test_frame_manager_simple_lru PROPERTIES WORKING_DIRECTORY [==[/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/unittest]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( bp_manager_test_TESTS test_frame_manager.test_frame_manager_simple_lru)
