set(_CMAKE_BUILDSYS_CONFIG_TEST_LTO_DIR ${CMAKE_CURRENT_LIST_DIR})

message(STATUS "cmake-buildsys: Testing if the compiler can do link-time-optimized builds...")

try_run(_RUN_RESULT_VAR _COMPILE_RESULT_VAR "${CMAKE_CURRENT_BINARY_DIR}"
  "${_CMAKE_BUILDSYS_CONFIG_TEST_LTO_DIR}/cmake-tests/test-lto.cpp"
  COMPILE_DEFINITIONS "${CMAKE_CXX_FLAGS} -flto")

if(NOT _COMPILE_RESULT_VAR)
  message(STATUS "cmake-buildsys: Link-time-optimization unsupported, disabling LTO builds")
  set(CMAKE_BUILDSYS_CONFIG_HAS_LTO_BUILDS OFF)
elseif(_RUN_RESULT_VAR)
  message(STATUS "cmake-buildsys: Link-time-optimization produces bad outputs, disabling LTO builds")
  set(CMAKE_BUILDSYS_CONFIG_HAS_LTO_BUILDS OFF)
else()
  message(STATUS "cmake-buildsys: Link-time-optimization supported, enabling LTO builds")
  set(CMAKE_BUILDSYS_CONFIG_HAS_LTO_BUILDS ON)
endif()
