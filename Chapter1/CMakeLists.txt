# Minimum required version of CMake
cmake_minimum_required(VERSION 3.22)

# Project name and version
project(Simple VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory to the search path
include_directories(${PROJECT_SOURCE_DIR}/Include)

# Add the executable target
add_library(simple_lib
                      src/parser.cc
                      # parser
                     src/node/constant_node.cc
                     src/node/node.cc
                     src/node/return_node.cc
                     src/node/start_node.cc)
add_subdirectory(tests)
add_executable(Simple src/main.cc)
target_link_libraries(Simple PUBLIC simple_lib)