# Minimum required version of CMake
cmake_minimum_required(VERSION 3.22)

# Project name and version
project(Simple VERSION 1.0)

enable_testing()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable AddressSanitizer for Clang or GCC in Debug builds
#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    message(STATUS "Enabling AddressSanitizer")
#
#    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#        # For Clang and GCC, enable AddressSanitizer
#        set(SANITIZER_FLAGS "-fsanitize=address")
#        add_compile_options(${SANITIZER_FLAGS})
#        add_link_options(${SANITIZER_FLAGS})
#    endif()
#endif()


# Add the include directory to the search path
include_directories(${PROJECT_SOURCE_DIR}/Include)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    message("using clang")
    set(DONSUS_USED_COMPILER "clang")
    add_compile_options(-fsanitize=address)
    #[[    add_compile_options(-Wunused-variable -Werror=unused-variable)]]
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    message("using gcc")
    set(DONSUS_USED_COMPILER "gcc")
    #[[    add_compile_options(-Wunused-variable -Werror=unused-variable)]]
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    message("using msvc")
    set(DONSUS_USED_COMPILER "msvc")
    add_compile_options(/W4 /WX)
endif ()

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
        src/node/new_node.cc
        src/node/mem_op_node.cc
        src/node/store_node.cc
        src/node/load_node.cc
        src/node/cast_node.cc

        # type
        src/type/integer_type.cc
        src/type/type.cc
        src/type/tuple_type.cc
        src/type/field.cc
        src/type/type_struct.cc
        src/type/type_mem.cc
        src/type/type_mem_ptr.cc

        # utils
        src/utils.cc
        src/graphVisualizer.cc
        src/graphEvaluator.cc
        src/iterPeeps.cc
        src/IR_Printer.cc
        )

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(tests)
add_executable(Simple src/main.cc)
target_link_libraries(Simple PUBLIC simple_lib)