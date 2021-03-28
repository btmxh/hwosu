cmake_minimum_required(VERSION 3.12)

project(hwosu C CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(glad
    glad/glad.c
)

include_directories(glad)
include_directories(include)
include_directories(lzma)
include_directories(x264/include)

add_executable(hwosu
    lzma/LzmaDec.c

    src/gl_utils.cpp
    src/io_utils.cpp
    src/osu_file.cpp
    src/osr_file.cpp
    src/renderer.cpp
    src/skin.cpp
    src/timer.cpp
    src/x264.cpp
    src/main.cpp
)

target_link_libraries(hwosu PUBLIC ${CMAKE_SOURCE_DIR}/x264/lib/libx264.lib)
target_link_libraries(hwosu PUBLIC glad)

find_path(STB_INCLUDE_DIRS "stb.h")
target_include_directories(hwosu PUBLIC ${STB_INCLUDE_DIRS} ${G2V_INCLUDE_DIRS})
