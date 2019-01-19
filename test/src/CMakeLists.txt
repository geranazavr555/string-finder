cmake_minimum_required(VERSION 3.9)
project(duplicate-scanner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS: "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=undefined,address -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")

find_package(Qt5Widgets REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(run-tests
        ../../src/string_finder/exceptions.h

        gtest/gtest-all.cc
        gtest/gtest.h
        internal_tests.cpp
        main.cpp generator.cpp generator.h)

qt5_use_modules(run-tests Widgets)
target_link_libraries(run-tests -lpthread)