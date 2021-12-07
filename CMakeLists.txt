
cmake_minimum_required(VERSION 3.10)
project(cjit)

set(CMAKE_C_COMPILER   gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-O3)

include_directories(src)
include_directories(third-part)
include_directories(third-part/mir)

file(GLOB cjit_srcs
  src/cjit/*.cc)
add_library(cjit ${cjit_srcs})


file(GLOB test_srcs
  src/test/*.cc)
add_executable(cjit-test ${test_srcs})
target_link_directories(cjit-test PRIVATE third-part/mir/build-arm64)
target_link_libraries(cjit-test cjit mir)


