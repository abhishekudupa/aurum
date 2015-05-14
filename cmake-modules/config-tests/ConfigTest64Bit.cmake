set(_CMAKE_BUILDSYS_CONFIG_TEST_64_BIT_DIR ${CMAKE_CURRENT_LIST_DIR})

message(STATUS "cmake-buildsys: Testing for 64 bit system...")

try_run(_RUN_RESULT_VAR _COMPILE_RESULT_VAR
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${_CMAKE_BUILDSYS_CONFIG_TEST_64_BIT_DIR}/cmake-tests/test-64-bit.cpp"
  COMPILE_DEFINITIONS "${CMAKE_CXX_FLAGS}")

if(_COMPILE_RESULT_VAR AND NOT _RUN_RESULT_VAR)
  message(STATUS "cmake-buildsys: 64 bit system test passed.")
  set(CMAKE_BUILDSYS_CONFIG_HAVE_64_BIT ON)
else()
  message(STATUS "cmake-buildsys: 64 bit system test failed.")
  set(CMAKE_BUILDSYS_CONFIG_HAVE_64_BIT OFF)
endif()
