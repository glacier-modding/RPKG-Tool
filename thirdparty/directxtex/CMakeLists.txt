cmake_minimum_required(VERSION 3.22)
project(DirectXTex)

set(CMAKE_CXX_STANDARD 17)

file(GLOB source_files
        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)
file(GLOB header_files
        "${CMAKE_CURRENT_LIST_DIR}/*.h"
)
file(GLOB compiled_shaders
        "${CMAKE_CURRENT_LIST_DIR}/Shaders/Compiled/*.inc"
)

add_library(DirectXTex ${source_files} ${header_files} ${compiled_shaders})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Compiled)
