cmake_minimum_required(VERSION 2.8)
project(vinbero_dummy)
set(CMAKE_BUILD_TYPE Debug)
macro(use_c99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(use_c99)
use_c99()
add_subdirectory(src)
add_subdirectory(test)
