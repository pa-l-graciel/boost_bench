cmake_minimum_required(VERSION 2.6)

#include(log.cmake)

set(CMAKE_DEFS "-std=c++11 -fno-rtti -fno-threadsafe-statics -fvisibility-inlines-hidden")
#set(CMAKE_DEFS "-std=c++11 -fno-threadsafe-statics -fvisibility-inlines-hidden")
#set(EXTRA_DEFS "-g -O2 -Wall -Wextra")
#set(EXTRA_DEFS "-Wall -Wextra -Wno-write-strings -Wno-unused-variable")
set(EXTRA_DEFS "-Wno-write-strings -Wno-unused-variable")
add_definitions(${CMAKE_DEFS})
add_definitions(${EXTRA_DEFS})


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories($ENV{BOOST_ROOT})
link_directories($ENV{BOOST_ROOT}/stage/lib)

#include_directories($ENV{BOOST_ROOT}/libs)
#find_library(boost_program_options /usr/local/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## system environment variable that is defined in ~/.profile
MESSAGE(STATUS "BOOST_ROOT:         " 
        $ENV{BOOST_ROOT}
    )



add_subdirectory(apps)
