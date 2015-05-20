cmake_minimum_required(VERSION 3.2.0)
project(aurum)

set(AURUM_VERSION_MAJOR 1)
set(AURUM_VERSION_MINOR 0)
set(AURUM_VERSION_PATCH 0)
set(AURUM_VERSION
  ${AURUM_VERSION_MAJOR}.${AURUM_VERSION_MINOR}.${AURUM_VERSION_PATCH})

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/cmake-modules;${CMAKE_CURRENT_LIST_DIR}/cmake-modules/config-tests")

set(AURUM_DEFAULT_CXX_FLAGS "-Wall -pedantic -std=c++11")

include(CheckCompiler)

# configure for the platform
include(ConfigTest64Bit)
include(ConfigTestSSE4.2)
include(ConfigTestLibrt)
include(ConfigTestLTO)
include(ConfigTestGDB)

if(CMAKE_BUILDSYS_CONFIG_HAVE_SSE4.2)
  set(AURUM_DEFAULT_CXX_FLAGS "${AURUM_DEFAULT_CXX_FLAGS} -msse4.2")
endif()

if(NOT CMAKE_BUILDSYS_CONFIG_HAVE_64_BIT)
  message(FATAL_ERROR "Only 64 bit platforms are currently supported!")
endif()

if(CMAKE_BUILDSYS_CONFIG_HAVE_LIBRT)
  set(AURUM_LINK_LIBS "-lrt")
  set(AURUM_CFG_HAVE_LIBRT_ ON)
endif()

if(CMAKE_BUILDSYS_CONFIG_HAVE_GDB)
  set(AURUM_CFG_HAVE_GDB_ ON)
  set(AURUM_CFG_PATH_TO_GDB_ "\"${CMAKE_BUILDSYS_CONFIG_PATH_TO_GDB}\"")
endif()

configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/config-includes/AurumConfig.h.in"
  "${CMAKE_BINARY_DIR}/AurumConfig.h")

# build types
set(CMAKE_CXX_FLAGS_RELEASE
  "${AURUM_DEFAULT_CXX_FLAGS} -O3"
  CACHE STRING "CXX compiler flags for release build" FORCE)

if(CMAKE_BUILDSYS_CONFIG_HAS_LTO_BUILDS)
  set(CMAKE_CXX_FLAGS_RELEASELTO
    "${AURUM_DEFAULT_CXX_FLAGS} -O3 -flto"
    CACHE STRING "CXX compiler flags for LTO release build" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG
  "${AURUM_DEFAULT_CXX_FLAGS} -ggdb3 -fnoinline -DAURUM_CFG_ASSERTIONS_ENABLED_"
  CACHE STRING "CXX COMPILER flags for debug build" FORCE)

mark_as_advanced(CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASELTO)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install" CACHE PATH "Default install path" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release
    CACHE STRING "Build Type: One of Release, ReleaseLTO, Debug" FORCE)
endif()

# option for enabling assertion checking
option(AURUM_CFG_ASSERTIONS_ENABLED_
  "Enable assertion checking."
  OFF)
option(AURUM_CFG_LOGGING_ENABLED_
  "Enable logging options."
  OFF)
option(AURUM_BUILD_TEST_CASES
  "Build test cases (unit tests) for libaurum"
  OFF)

# locations for built objects
set(AURUM_BIN_DIR "bin")
set(AURUM_LIB_DIR "lib")
set(AURUM_INC_DIR "include/aurum")

# all the source files that need to be compiled
set(AURUM_CXX_SOURCE_FILES
  src/allocators/MemoryManager.cpp
  src/allocators/PoolAllocator.cpp
  src/allocators/SmallBlockAllocator.cpp

  src/basetypes/AurumErrors.cpp
  src/basetypes/AurumTypes.cpp

  src/containers/BitSet.cpp

  src/hashfuncs/CityHash.cpp
  src/hashfuncs/FNVHash.cpp
  src/hashfuncs/MurmurHash.cpp
  src/hashfuncs/SpookyHash.cpp

  src/logging/LogManager.cpp

  src/primeutils/PrecomputedPrimeList.cpp
  src/primeutils/PrimeGenerator.cpp

  src/progoptions/ProgramOptions.cpp
  src/progoptions/OptionMap.cpp
  src/progoptions/OptionValue.cpp

  src/strutils/StrUtils.cpp

  src/timers/Timers.cpp)

add_library(aurum-shared SHARED
  ${AURUM_CXX_SOURCE_FILES})

add_library(aurum-static STATIC
  ${AURUM_CXX_SOURCE_FILES})

target_link_libraries(aurum-shared ${AURUM_LINK_LIBS})
target_link_libraries(aurum-static ${AURUM_LINK_LIBS})
target_include_directories(aurum-shared PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(aurum-static PRIVATE ${CMAKE_BINARY_DIR})

# set the interface include directories for projects which include
# this project as a subproject
set_property(TARGET aurum-shared APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}>
  $<INSTALL_INTERFACE:include/aurum>)
set_property(TARGET aurum-static APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}>
  $<INSTALL_INTERFACE:include/aurum>)

set_target_properties(aurum-shared PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AURUM_LIB_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AURUM_LIB_DIR}"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AURUM_BIN_DIR}")

set_target_properties(aurum-static PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AURUM_LIB_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AURUM_LIB_DIR}"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AURUM_BIN_DIR}")

# testing
if(AURUM_BUILD_TEST_CASES)
  add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/thirdparty/google-test")
  file(GLOB AURUM_TEST_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/tests/unit-tests/*.cpp")
  add_executable(aurum-unit-tests-shared "${AURUM_TEST_SOURCE_FILES}")
  add_dependencies(aurum-unit-tests-shared gtest)
  add_dependencies(aurum-unit-tests-shared aurum-shared)
  target_link_libraries(aurum-unit-tests-shared gtest gtest_main)
  target_link_libraries(aurum-unit-tests-shared ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(aurum-unit-tests-shared aurum-shared)
  target_include_directories(aurum-unit-tests-shared PRIVATE "${CMAKE_CURRENT_LIST_DIR}/thirdparty/google-test/include")
  set_target_properties(aurum-unit-tests-shared PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AURUM_BIN_DIR}")

  add_executable(aurum-unit-tests-static "${AURUM_TEST_SOURCE_FILES}")
  add_dependencies(aurum-unit-tests-static gtest)
  add_dependencies(aurum-unit-tests-static aurum-static)
  target_link_libraries(aurum-unit-tests-static gtest gtest_main)
  target_link_libraries(aurum-unit-tests-static ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(aurum-unit-tests-static aurum-static)
  target_include_directories(aurum-unit-tests-static PRIVATE "${CMAKE_CURRENT_LIST_DIR}/thirdparty/google-test/include")
  set_target_properties(aurum-unit-tests-static PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AURUM_BIN_DIR}")
endif()

# installation
install(TARGETS aurum-static
  ARCHIVE DESTINATION ${AURUM_LIB_DIR} COMPONENT dev)

install(TARGETS aurum-shared
  LIBRARY DESTINATION ${AURUM_LIB_DIR} COMPONENT dev)

install(DIRECTORY
  src/allocators
  src/basetypes
  src/containers
  src/hashfuncs
  src/logging
  src/memory
  src/primeutils
  src/progoptions
  src/strutils
  src/timers
  DESTINATION ${AURUM_INC_DIR}
  COMPONENT dev
  FILES_MATCHING PATTERN "*.hpp")

install(FILES ${CMAKE_BINARY_DIR}/AurumConfig.h
  DESTINATION ${AURUM_INC_DIR}
  COMPONENT dev)
