cmake_minimum_required(VERSION 3.8)
project(simplenc)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES simplenc.c simplenc.h)
add_library(simplenc ${SOURCE_FILES})