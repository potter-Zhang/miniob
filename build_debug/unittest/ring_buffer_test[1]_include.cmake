if(EXISTS "/home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest/ring_buffer_test[1]_tests.cmake")
  include("/home/someone/桌面/myd/miniob合并/sdk_miniob/build/unittest/ring_buffer_test[1]_tests.cmake")
else()
  add_test(ring_buffer_test_NOT_BUILT ring_buffer_test_NOT_BUILT)
endif()
