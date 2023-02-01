cmake_minimum_required(VERSION 3.20)

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
  message(STATUS "Setting default vcpkg target triplet to $ENV{VCPKG_DEFAULT_TRIPLET}")
  set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_DEFAULT_TRIPLET})
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  message(STATUS "VCPKG found: $ENV{VCPKG_ROOT}")
  message(STATUS "Using VCPKG integration")
  message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
elseif(DEFINED CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")
  if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
    message(STATUS "Toolchain uses VCPKG integration")
    message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
  endif()
else()
  message(WARNING "vcpkg not found, toolchain not defined, build might fail")
endif()

project(physycom-utils VERSION 1.0.0 LANGUAGES CXX)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Install prefix" FORCE)
endif()

set(INSTALL_BIN_DIR      "bin"                   CACHE PATH "Path where exe and dll will be installed")
set(INSTALL_LIB_DIR      "lib"                   CACHE PATH "Path where lib will be installed")
set(INSTALL_INCLUDE_DIR  "include"               CACHE PATH "Path where headers will be installed ")
set(INSTALL_CMAKE_DIR    "share/${PROJECT_NAME}" CACHE PATH "Path where cmake configs will be installed")

find_package(Boost    REQUIRED)
find_package(jsoncons REQUIRED)

add_executable(histograms   "${CMAKE_SOURCE_DIR}/src/histograms.cpp")
add_executable(average      "${CMAKE_SOURCE_DIR}/src/average.cpp")
add_executable(simultaneous "${CMAKE_SOURCE_DIR}/src/simultaneous.cpp")

target_link_libraries(histograms   PRIVATE Boost::boost jsoncons)
target_link_libraries(average      PRIVATE Boost::boost)
target_link_libraries(simultaneous PRIVATE Boost::boost)

install(TARGETS histograms average simultaneous
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)
