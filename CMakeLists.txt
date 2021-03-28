cmake_minimum_required(VERSION 3.12)

project(hwosu C CXX)

set(CMAKE_CXX_STANDARD 17)

option(G2V_USE_FFMPEG_ENCODER "" ON)

add_library(glad
    glad/glad.c
)

include(${CMAKE_CURRENT_LIST_DIR}/../g2vpp/CMakeLists.txt)

include_directories(glad)
include_directories(include)
include_directories(lzma)

add_executable(hwosu
    lzma/LzmaDec.c

    src/gl_utils.cpp
    src/io_utils.cpp
    src/osu_file.cpp
    src/osr_file.cpp
    src/renderer.cpp
    src/skin.cpp
    src/timer.cpp
    src/main.cpp
)

target_link_libraries(hwosu PRIVATE g2vpp)

find_path(STB_INCLUDE_DIRS "stb.h")
target_include_directories(hwosu PUBLIC ${STB_INCLUDE_DIRS} ${G2V_INCLUDE_DIRS})
