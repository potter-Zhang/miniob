if(EXISTS "/home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest/pidfile_test[1]_tests.cmake")
  include("/home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest/pidfile_test[1]_tests.cmake")
else()
  add_test(pidfile_test_NOT_BUILT pidfile_test_NOT_BUILT)
endif()
