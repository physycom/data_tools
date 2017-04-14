cmake_minimum_required (VERSION 2.8)
project (data_tools)
set (CMAKE_CXX_STANDARD 11)

set(JSONCONS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/jsoncons/src)

if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_USE_MATH_DEFINES)
endif()

find_package(Boost REQUIRED)
	
include_directories(${Boost_INCLUDE_DIRS} ${JSONCONS_INCLUDE_DIR} ${OTHER_INCLUDE_DIR} ${PROJECT_BINARY_DIR})

add_executable (histograms "${CMAKE_SOURCE_DIR}/src/histograms.cpp")
add_executable (average "${CMAKE_SOURCE_DIR}/src/average.cpp")
add_executable (simultaneous "${CMAKE_SOURCE_DIR}/src/simultaneous.cpp")

install (TARGETS histograms average simultaneous
	DESTINATION ${CMAKE_SOURCE_DIR}/bin)



