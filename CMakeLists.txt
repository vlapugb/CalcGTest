cmake_minimum_required(VERSION 3.22)
project(CalcGTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_library(calc_lib
        ICalculator.h SimpleCalculator.h SimpleCalculator.cpp
        IHistory.h InMemoryHistory.h InMemoryHistory.cpp
)
target_include_directories(calc_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(calc_tests calculator_tests.cpp)
target_link_libraries(calc_tests PRIVATE calc_lib GTest::gtest_main GTest::gmock)

enable_testing()
include(GoogleTest)
gtest_discover_tests(calc_tests)

target_compile_options(calc_lib PRIVATE --coverage)
target_link_options(calc_lib PRIVATE --coverage)
target_compile_options(calc_tests PRIVATE --coverage)
target_link_options(calc_tests PRIVATE --coverage)
