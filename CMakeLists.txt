cmake_minimum_required(VERSION 3.15)
project(parallel_bfs VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
        parlaylib
        GIT_REPOSITORY https://github.com/cmuparlay/parlaylib.git
        GIT_TAG master
)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps CACHE PATH "Base directory for fetched content")

FetchContent_GetProperties(parlaylib)
if(NOT parlaylib_POPULATED)
    message(STATUS "Fetching parlaylib...")
    FetchContent_Populate(parlaylib)
    message(STATUS "Parlaylib fetched to: ${parlaylib_SOURCE_DIR}")

    add_subdirectory(${parlaylib_SOURCE_DIR} ${parlaylib_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_executable(speed_measure
        main.cpp
        src/seqbfs.cpp
        src/parbfs.cpp
)

target_include_directories(speed_measure PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(speed_measure PRIVATE
        parlay
        Threads::Threads
)

if(WIN32)
    target_compile_definitions(speed_measure PRIVATE
            NOMINMAX
            _CRT_SECURE_NO_WARNINGS
    )
endif()