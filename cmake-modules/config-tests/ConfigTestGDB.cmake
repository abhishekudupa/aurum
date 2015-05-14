set(_CMAKE_BUILDSYS_CONFIG_TEST_GDB_DIR ${CMAKE_CURRENT_LIST_DIR})

execute_process(COMMAND "which" "gdb" RESULT_VARIABLE _RESULT_VARIABLE
  OUTPUT_VARIABLE _OUT_VAR ERROR_VARIABLE _OUT_VAR)
string(STRIP ${_OUT_VAR} _PATH_TO_GDB)
if(RESULT_VARIABLE)
  message(STATUS "cmake-buildsys: No gdb found, disabling debug feaures on all builds")
  set(CMAKE_BUILDSYS_CONFIG_HAVE_GDB OFF)
else()
  message(STATUS "cmake-buildsys: Using gdb: \"${_PATH_TO_GDB}\".")
  set(CMAKE_BUILDSYS_CONFIG_HAVE_GDB ON)
  set(CMAKE_BUILDSYS_CONFIG_PATH_TO_GDB ${_PATH_TO_GDB})
endif()
