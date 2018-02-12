cmake_minimum_required (VERSION 2.8)
project (data_tools)

if (EXISTS "$ENV{WORKSPACE}/sysconfig/cmake/physycom_config.cmake")
   message(STATUS "Enabling physycom settings")
   include("$ENV{WORKSPACE}/sysconfig/cmake/physycom_config.cmake")
else()
   message(STATUS "Unable to find physycom settings file")
endif()


set(JSONCONS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/jsoncons/include)

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${JSONCONS_INCLUDE_DIR})

add_executable (histograms "${CMAKE_SOURCE_DIR}/src/histograms.cpp")
add_executable (average "${CMAKE_SOURCE_DIR}/src/average.cpp")
add_executable (simultaneous "${CMAKE_SOURCE_DIR}/src/simultaneous.cpp")

install(TARGETS histograms average simultaneous DESTINATION ${CMAKE_SOURCE_DIR}/bin)
