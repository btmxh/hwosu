cmake_minimum_required(VERSION 3.12)

project(hwosu C CXX)

set(CMAKE_CXX_STANDARD 17)

option(G2V_USE_FFMPEG_ENCODER "" ON)

add_library(glad
    glad/glad.c
)

# include_directories(../gl2vid)
include(../gl2vid/CMakeLists.txt)

include_directories(glad)
include_directories(include)
include_directories(lzma)

add_executable(hwosu
    lzma/LzmaDec.c

    src/io_utils.cpp
    src/osu_file.cpp
    src/osr_file.cpp
    src/main.cpp
)

target_compile_definitions(hwosu PUBLIC ${gl2vid_DEFINITIONS})
target_include_directories(hwosu PUBLIC ${gl2vid_INCLUDE_DIR})
target_link_libraries(hwosu PUBLIC gl2vid)
