set(_CMAKE_BUILDSYS_CONFIG_TEST_SSE_4_2_DIR ${CMAKE_CURRENT_LIST_DIR})

message(STATUS "cmake-buildsys: Testing for SSE 4.2...")

execute_process(COMMAND "${_CMAKE_BUILDSYS_CONFIG_TEST_SSE_4_2_DIR}/cmake-tests/test-sse-4.2.py"
  RESULT_VARIABLE _RESULT_VARIABLE)
if(NOT _RESULT_VARIABLE)
  message(STATUS "cmake-buildsys: Platform supports SSE 4.2.")
  set(CMAKE_BUILDSYS_CONFIG_HAVE_SSE_4_2 ON)
else()
  set(CMAKE_BUILDSYS_CONFIG_HAVE_SSE_4_2 OFF)
  message(STATUS "cmake-buildsys: Platform does NOT support SSE 4.2, not using SSE 4.2.")
endif()
