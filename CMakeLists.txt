cmake_minimum_required(VERSION 3.11)

project(rolling_grid)

add_library(rgrid INTERFACE)
target_include_directories(rgrid INTERFACE include)

add_executable(test src/main.cpp)

target_link_libraries(test rgrid)