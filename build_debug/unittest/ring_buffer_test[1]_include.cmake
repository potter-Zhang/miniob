if(EXISTS "/home/harry/Desktop/miniob_test/build/unittest/ring_buffer_test[1]_tests.cmake")
  include("/home/harry/Desktop/miniob_test/build/unittest/ring_buffer_test[1]_tests.cmake")
else()
  add_test(ring_buffer_test_NOT_BUILT ring_buffer_test_NOT_BUILT)
endif()
