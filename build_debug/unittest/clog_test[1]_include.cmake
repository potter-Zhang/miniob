if(EXISTS "/home/harry/Desktop/miniob/build/unittest/clog_test[1]_tests.cmake")
  include("/home/harry/Desktop/miniob/build/unittest/clog_test[1]_tests.cmake")
else()
  add_test(clog_test_NOT_BUILT clog_test_NOT_BUILT)
endif()
