if(EXISTS "/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/unittest/arithmetic_expression_test[1]_tests.cmake")
  include("/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/unittest/arithmetic_expression_test[1]_tests.cmake")
else()
  add_test(arithmetic_expression_test_NOT_BUILT arithmetic_expression_test_NOT_BUILT)
endif()
