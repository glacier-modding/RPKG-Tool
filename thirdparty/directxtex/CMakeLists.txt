cmake_minimum_required(VERSION 3.22)
project(DirectXTex)

set(CMAKE_CXX_STANDARD 17)

file(GLOB DXTEX_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)
file(GLOB DXTEX_HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/*.h"
)
file(GLOB DXTEX_PRECOMPILED_SHADERS
        "${CMAKE_CURRENT_LIST_DIR}/Shaders/Compiled/*.inc"
)

add_library(DirectXTex ${DXTEX_SOURCE_FILES} ${DXTEX_HEADERS} ${DXTEX_PRECOMPILED_SHADERS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Shaders/Compiled)
