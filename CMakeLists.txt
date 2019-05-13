cmake_minimum_required(VERSION 3.12)
project(HackGlibcFILE)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
add_executable(HackGlibcFILE hack.c hack.h main.cpp)