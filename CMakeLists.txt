cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(log VERSION 1.0 DESCRIPTION "Small log library")

# Source list
add_library(log SHARED src/log.cpp)

# Public API
set_target_properties(log PROPERTIES PUBLIC_HEADER include/log.h)

# Include dir
target_include_directories(log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(log PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(log PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
