cmake_minimum_required(VERSION 3.12.0)

project(toolbox)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FetchDependency)

fetch_dependency(vlog "https://git.martian.ag/open-source/vlog.git" "master")

find_package(hjson REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -Werror \
    -Wall \
    -Wextra \
    -Weverything \
    -Werror=return-type \
    -Wno-c++98-compat \
    -Wno-c++98-compat-pedantic \
    -Wno-ctad-maybe-unsupported \
    -Wno-exit-time-destructors \
    -Wno-double-promotion")

# Disable additional warnings to get stb_image_write to compile
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -Wno-old-style-cast \
    -Wno-sign-conversion \
    -Wno-implicit-fallthrough \
    -Wno-missing-variable-declarations \
    -Wno-zero-as-null-pointer-constant \
    -Wno-cast-qual \
    -Wno-reserved-id-macro \
    -Wno-cast-align \
    -Wno-missing-prototypes \
    -Wno-extra-semi-stmt \
    -Wno-implicit-int-conversion \
    -Wno-comma \
    -Wno-disabled-macro-expansion \
    -Wno-padded \
    -Wno-unused-parameter")

add_library(
  toolbox SHARED src/asyncapp.cpp src/file_utils.cpp src/hjson_helper.cpp
  src/unix_helper.cc src/task.cpp src/taskthread.cpp src/termtool.cpp
  src/tictoc.cpp src/perftimer.cpp src/time_format.cpp src/rate.cpp src/socket.cpp src/box_utils.cpp)

target_include_directories(toolbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/toolbox)
target_include_directories(toolbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(toolbox PUBLIC hjson vlog pthread)
target_link_libraries(toolbox PRIVATE stdc++fs)
set(toolbox_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "toolbox source path" FORCE)
set(toolbox_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "toolbox lib path" FORCE)
install(TARGETS toolbox DESTINATION lib)
option(TOOLBOX_TESTS "Enable tests for toolbox" OFF)

if (${ENABLE_ALL_TESTS})
  set(TOOLBOX_TESTS ON)
endif()

# only build the library tests when building the library
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  set(TOOLBOX_TESTS ON)
endif()

if (${TOOLBOX_TESTS})
  enable_testing()
  add_subdirectory(tests)
endif()

set(toolbox_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "toolbox_SOURCE_DIR")
