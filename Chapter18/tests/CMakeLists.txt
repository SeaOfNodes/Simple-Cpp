cmake_minimum_required(VERSION 3.22)
project(simple_test)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_BUILD_TYPE Debug)

include(GoogleTest)
add_executable(simple_test Chapter17Test.cc TypeTest.cc)

target_link_libraries(simple_test PUBLIC
        GTest::gtest_main
        simple_lib

)