cmake_minimum_required(VERSION 3.24)
project(WinApi_obfuscation)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(WinApi_obfuscation
        main.cpp
        winapi.cpp
        winapi.h
        winapi_hash.h
        winapi_type.h)
