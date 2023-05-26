cmake_minimum_required(VERSION 3.23)
project(WinApi_obfuscation)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(WinApi_obfuscation
        main.cpp
        winapi.cpp
        winapi.h
        winapi_hash.h
        )
