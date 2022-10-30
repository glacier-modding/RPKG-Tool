cmake_minimum_required(VERSION 3.5)
project(GLTFSDK)

file(GLOB GLTFSDK_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/*.cpp"
)
file(GLOB GLTFSDK_GEN_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Inc/*.h"
)
file(GLOB GLTFSDK_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Inc/GLTFSDK/*.h"
)

add_library(GLTFSDK ${GLTFSDK_SOURCE_FILES} ${GLTFSDK_GEN_HEADER_FILES} ${GLTFSDK_HEADER_FILES})

if (MSVC)
    # Generate PDB files in all configurations, not just Debug (/Zi)
    # Set warning level to 4 (/W4)
    target_compile_options(GLTFSDK PRIVATE "/Zi;/EHsc")

    # Make sure that all PDB files on Windows are installed to the output folder with the libraries. By default, only the debug build does this.
    set_target_properties(GLTFSDK PROPERTIES COMPILE_PDB_NAME "GLTFSDK" COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_DIRECTORY}")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(GLTFSDK
        PRIVATE "-Wunguarded-availability"
        PRIVATE "-Wall"
        PRIVATE "-Werror"
        PUBLIC "-Wno-unknown-pragmas")
endif()

target_include_directories(GLTFSDK
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/Inc"
    PRIVATE "${CMAKE_SOURCE_DIR}/Built/Int"
)
