include(CheckLibraryExists)

message(STATUS "cmake-buildsys: Checking for librt")
CHECK_LIBRARY_EXISTS(rt timer_create "time.h" CMAKE_BUILDSYS_CONFIG_HAVE_LIBRT)
if(NOT CMAKE_BUILDSYS_CONFIG_HAVE_LIBRT)
  message(STATUS "cmake-buildsys: Could not find librt.")
else()
  message(STATUS "cmake-buildsys: librt works fine.")
endif()
