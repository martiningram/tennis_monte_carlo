# Martin Ingram. Copyright (C) 2015.
# With thanks to George Barnett whose CMakeLists.txt heavily inspired this
# CMakeLists file.

#
# Project Properties
#
cmake_minimum_required(VERSION 2.6.2)
project("tennis_monte_carlo")
set(APPLICATION_NAME "Monte Carlo Tennis")
set(APPLICATION_CODENAME "${PROJECT_NAME}")
set(APPLICATION_VERSION_MAJOR 1)
set(APPLICATION_VERSION_MINOR 0)


# Configuration options
option(WITH_TESTS "WITH_TESTS" OFF)

# Project Output Paths
set(MAINFOLDER ${PROJECT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${MAINFOLDER})
set(LIBRARY_OUTPUT_PATH "${MAINFOLDER}/lib")
set(project_BIN ${PROJECT_NAME})
set(project_LIB "lib${PROJECT_NAME}")

# Project search paths
list(APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}/thirdparty")

# Include directory
include_directories("${MAINFOLDER}/include")

# Compiler flags
set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -g -Wall -pedantic ${CMAKE_CXX_FLAGS}")

# If we're building with tests.
if(WITH_TESTS)
  message(STATUS "Enabling tests")
  add_subdirectory("${MAINFOLDER}/thirdparty/gtest")
  include_directories("${gtest_SOURCE_DIR}/include" ${gtest_SOURCE_DIR})
  add_subdirectory(tests)
endif()


# Build targets
add_subdirectory(src)

# Install targets:
if (EXISTS "${MAINFOLDER}/include/${PROJECT_NAME}" AND IS_DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}")
    install(DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}" DESTINATION "include")
endif (EXISTS "${MAINFOLDER}/include/${PROJECT_NAME}" AND IS_DIRECTORY "${MAINFOLDER}/include/${PROJECT_NAME}")

