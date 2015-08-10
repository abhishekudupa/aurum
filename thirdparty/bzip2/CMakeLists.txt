cmake_minimum_required(VERSION 2.8.0)
project(bzip2)

set(CMAKE_AR "gcc-ar")
set(CMAKE_RANLIB "gcc-ranlib")
set(CMAKE_NM "gcc-nm")

set(BZLIB_C_SRC_FILES
  src/blocksort.c
  src/huffman.c
  src/crctable.c
  src/randtable.c
  src/compress.c
  src/decompress.c
  src/bzlib.c
)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS_RELEASE "-O3 -D_FILE_OFFSET_BITS=64")

add_library(bzip2-shared SHARED ${BZLIB_C_SRC_FILES})
add_library(bzip2-static STATIC ${BZLIB_C_SRC_FILES})

set_property(TARGET bzip2-shared APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>)
set_property(TARGET bzip2-static APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>)

set_target_properties(bzip2-shared PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set_target_properties(bzip2-static PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

install(TARGETS bzip2-static
  ARCHIVE DESTINATION lib COMPONENT dev)

install(TARGETS bzip2-shared
  LIBRARY DESTINATION lib COMPONENT dev)

install(FILES
  src/bzlib.h
  DESTINATION include
  COMPONENT dev)
