if(EXISTS "/home/harry/Desktop/miniob_test/build/unittest/arithmetic_expression_test[1]_tests.cmake")
  include("/home/harry/Desktop/miniob_test/build/unittest/arithmetic_expression_test[1]_tests.cmake")
else()
  add_test(arithmetic_expression_test_NOT_BUILT arithmetic_expression_test_NOT_BUILT)
endif()
