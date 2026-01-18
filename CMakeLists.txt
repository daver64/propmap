
##
## Propmap Project Details
##
## (c) 2024 David Rowbotham (thedaver64@gmail.com)
##
##
cmake_minimum_required(VERSION 3.0...3.22)
project("propmap")
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "64 bit compiler required.")
endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

##
## MSVC Compile Flags
##
if(MSVC)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "/std:c++20 /EHsc /Qpar /GS- /O2 /D NDEBUG")
endif(MSVC)


##
## Linux Compile Flags
##
if(UNIX)
set(CMAKE_CXX_FLAGS "-std=c++20 -O2")
endif(UNIX)


##
## MINGW.
##
if(MINGW)
set(CMAKE_CXX_FLAGS "-std=c++20 -O2")
endif(MINGW)


##
## Project Build.
##
add_executable(propmap
    src/demo.cpp
)

target_include_directories(propmap PRIVATE "${CMAKE_SOURCE_DIR}/include")

##
## MSVC Platform Link Options
##
if(MSVC)
set_target_properties(propmap PROPERTIES LINK_FLAGS "/LIBPATH:${CMAKE_SOURCE_DIR}/lib/win64 /ENTRY:mainCRTStartup /SUBSYSTEM:CONSOLE")
endif(MSVC)

