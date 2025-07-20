cmake_minimum_required(VERSION 3.15)

project(flightSim_playground VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Dependencies
#find_package(cpr CONFIG REQUIRED)
#find_package(fmt CONFIG REQUIRED)
set ( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/lib/cmake)
find_package ( cpr REQUIRED )
include_directories ( ${CPR_INCLUDE_DIRS} )

add_executable(flightSim_playground main.cpp)

# flightSim sources and headers
file(GLOB_RECURSE flightSim_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

target_sources(flightSim_playground PRIVATE ${flightSim_SOURCES})

# flightSim include dirs
target_include_directories(flightSim_playground PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(flightSim_playground PRIVATE 
    cpr::cpr
)

# Set configuration properties for flightSim
if(MSVC)
    target_compile_options(flightSim_playground PRIVATE
        /W3
        /MP # multithreaded build
        /WX # warnings as errors
        /ZI # program database for edit and continue
    )
else()
    target_compile_options(flightSim_playground PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()
