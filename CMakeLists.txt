cmake_minimum_required (VERSION 2.8)
project (data_tools)
set (CMAKE_CXX_STANDARD 11)

set(JSONCONS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/jsoncons/src)

### Set output directories on a per-configuration base
# Single configuration
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{HOME}/bin )
# Multi configuration
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    if( OUTPUTCONFIG STREQUAL "RELEASE" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} $ENV{HOME}/bin )
    else()
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${OUTPUTCONFIG} )
    endif()
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_USE_MATH_DEFINES)
endif()

find_package(Boost REQUIRED)
	
include_directories(${Boost_INCLUDE_DIRS} ${JSONCONS_INCLUDE_DIR})

add_executable (histograms "${CMAKE_SOURCE_DIR}/src/histograms.cpp")
add_executable (average "${CMAKE_SOURCE_DIR}/src/average.cpp")
add_executable (simultaneous "${CMAKE_SOURCE_DIR}/src/simultaneous.cpp")

