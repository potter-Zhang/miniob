if(EXISTS "/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/unittest/md5_test[1]_tests.cmake")
  include("/home/someone/桌面/myd/miniob-2023/sdk_miniob/build/unittest/md5_test[1]_tests.cmake")
else()
  add_test(md5_test_NOT_BUILT md5_test_NOT_BUILT)
endif()