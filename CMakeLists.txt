
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

##
## MSVC Compile Flags
##
if(MSVC)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "/std:c++20 -I../include /EHsc /Qpar /GS- /O2 /D NDEBUG")
endif(MSVC)


##
## Linux Compile Flags
##
if(UNIX)
set(CMAKE_CXX_FLAGS "-std=c++20 -I../include -O2")
endif(UNIX)


##
## MINGW. Not supported atm.
##
if(MINGW)
set(CMAKE_CXX_FLAGS "-I../include -O2")
endif(MINGW)


##
## Project Build.
##
add_executable(propmap
    src/demo.cpp
)

##
## MSVC Platform Link Options
##
if(MSVC)
set_target_properties(propmap PROPERTIES LINK_FLAGS "/LIBPATH:../lib/win64 /ENTRY:mainCRTStartup /SUBSYSTEM:CONSOLE")
endif(MSVC)

