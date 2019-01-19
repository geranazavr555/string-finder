cmake_minimum_required(VERSION 3.9)
project(string-finder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS: "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=undefined,address -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")

find_package(Qt5Widgets REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(string-finder
        ui/main.cpp
        ui/mainwindow.cpp
        ui/mainwindow.h
        ui/mainwindow.ui
        string_finder/exceptions.h
        string_finder/text_deteminer.cpp string_finder/text_deteminer.h string_finder/general_declarations.cpp string_finder/general_declarations.h string_finder/index.cpp string_finder/index.h string_finder/search.cpp string_finder/search.h string_finder/worker.cpp string_finder/worker.h)
qt5_use_modules(string-finder Widgets)
target_link_libraries(string-finder -lpthread)
