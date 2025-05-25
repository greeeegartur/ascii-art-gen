cmake_minimum_required(VERSION 3.31)
project(ascii_art_gen)

set(CMAKE_CXX_STANDARD 17)

include_directories(project)

add_executable(ascii_art_gen
        project/ascii_art.cpp
        project/stb_image.h
        project/stb_image_resize2.h
        project/gif_load.h)
