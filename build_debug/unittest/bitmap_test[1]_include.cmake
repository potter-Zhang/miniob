if(EXISTS "/home/harry/Desktop/miniob_test/build/unittest/bitmap_test[1]_tests.cmake")
  include("/home/harry/Desktop/miniob_test/build/unittest/bitmap_test[1]_tests.cmake")
else()
  add_test(bitmap_test_NOT_BUILT bitmap_test_NOT_BUILT)
endif()
