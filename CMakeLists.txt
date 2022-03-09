cmake_minimum_required(VERSION 3.5)

project(conet_top)

add_library(conet_top INTERFACE)

enable_testing()
add_subdirectory(conet)
add_subdirectory(example)
add_subdirectory(test)
