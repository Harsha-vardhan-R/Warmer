if(EXISTS "/home/harsha/Documents/CPP_PROJECTS/pamplejuce/Tests-b12d07c_tests.cmake")
  include("/home/harsha/Documents/CPP_PROJECTS/pamplejuce/Tests-b12d07c_tests.cmake")
else()
  add_test(Tests_NOT_BUILT-b12d07c Tests_NOT_BUILT-b12d07c)
endif()