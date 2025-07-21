cmake_minimum_required(VERSION 3.15)

project(flightSim_playground VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Dependencies
find_package(cpr CONFIG REQUIRED)
#find_package(fmt CONFIG REQUIRED)

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
    "C:/MSFS2024SDK/SimConnectSDK/include"
)

target_link_libraries(flightSim_playground PRIVATE cpr::cpr
    "C:/MSFS2024SDK/SimConnectSDK/lib/SimConnect.lib"
    shlwapi.lib
    user32.lib
    Ws2_32.lib
)

add_custom_command(TARGET flightSim_playground POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/MSFS2024SDK/SimConnectSDK/lib/SimConnect.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/build/Release"
)

target_compile_definitions(flightSim_playground PRIVATE SIMCONNECT_DEBUG)

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
