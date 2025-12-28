cmake_minimum_required(VERSION 3.15)
project(parallel_bfs VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
        parlaylib
        GIT_REPOSITORY https://github.com/cmuparlay/parlaylib.git
        GIT_TAG master
)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps CACHE PATH "Base directory for fetched content")

FetchContent_MakeAvailable(parlaylib)

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