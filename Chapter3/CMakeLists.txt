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
                     # nodes
                     src/node/add_node.cc
                     src/node/constant_node.cc
                     src/node/divnode.cc
                     src/node/minus_node.cc
                     src/node/mul_node.cc
                     src/node/node.cc
                     src/node/return_node.cc
                     src/node/start_node.cc
                     src/node/sub_node.cc

                    src/node/scope_node.cc
                    # type
                    src/type/integer_type.cc
                    src/type/type.cc

                    # utils
                    src/utils.cc
        src/graphVisualizer.cc)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(tests)
add_executable(Simple src/main.cc)
target_link_libraries(Simple PUBLIC simple_lib)