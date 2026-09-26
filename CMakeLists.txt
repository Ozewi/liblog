cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(liblog
        VERSION 1.1
        DESCRIPTION "A small log library"
       )

# Source list
add_library(liblog SHARED
            src/log.cpp)

set_property(
        TARGET liblog PROPERTY
        EXPORT_NAME log
)

# Public API
set_target_properties(liblog PROPERTIES PUBLIC_HEADER include/liblog/log.h)
set_target_properties(liblog PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(liblog PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# Include dir
target_include_directories(liblog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Include dir in installations
install(TARGETS liblog
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
       )
