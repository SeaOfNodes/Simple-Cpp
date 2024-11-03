# Minimum required version of CMake
cmake_minimum_required(VERSION 3.22)

# Project name and version
project(Simple VERSION 1.0)

enable_testing()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
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
        src/node/if_node.cc
        src/node/return_node.cc
        src/node/start_node.cc
        src/node/sub_node.cc
        src/node/bool_node.cc
        src/node/proj_node.cc
        src/node/not_node.cc
        src/node/multi_node.cc
        src/node/scope_node.cc
        src/node/region_node.cc
        src/node/phi_node.cc
        src/node/stop_node.cc
        src/node/loop_node.cc

        # type
        src/type/integer_type.cc
        src/type/type.cc
        src/type/tuple_type.cc

        # utils
        src/utils.cc
        src/graphVisualizer.cc
        src/graphEvaluator.cc)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(tests)
add_executable(Simple src/main.cc)
target_link_libraries(Simple PUBLIC simple_lib)