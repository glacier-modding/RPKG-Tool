cmake_minimum_required(VERSION 3.22)
project(rpkg VERSION 2.32.1)
include(FetchContent)

# We need to set the defaults as setting the policy will not
# persist when adding subdirectories.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0097 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0097 NEW)
cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 17)

# rapidjson is here to allow GLTFSDK to use it.
set(RAPIDJSON_BUILD_DOC OFF)
set(RAPIDJSON_BUILD_EXAMPLES OFF)
set(RAPIDJSON_BUILD_TESTS OFF)
FetchContent_Declare(rjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG        516d0473949fdcf0a6dc9fbb40fa92b3b85db184
    GIT_SUBMODULES ""
)

# GLTFSDK
FetchContent_Declare(gltfsdk
    GIT_REPOSITORY https://github.com/microsoft/glTF-SDK.git
    GIT_TAG        r1.9.6.0
)

# DirectXTex
set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)
FetchContent_Declare(directxtex
    GIT_REPOSITORY  https://github.com/microsoft/DirectXTex.git
    GIT_TAG         jan2023
)

set(ENABLE_UNIT_TESTS OFF)
set(ENABLE_SAMPLES OFF)
FetchContent_Declare(directxmath
    GIT_REPOSITORY  https://github.com/microsoft/DirectXMath.git
    GIT_TAG         dec2022
)

# nlohmann json
FetchContent_Declare(json
    URL         https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    URL_HASH    SHA256=8c4b26bf4b422252e13f332bc5e388ec0ab5c3443d24399acb675e68278d341f
)

# lz4
set(LZ4_BUILD_CLI OFF)
set(LZ4_BUILD_LEGACY_LZ4C OFF)
set(LZ4_BUNDLED_MODE ON)
FetchContent_Declare(lz4
    GIT_REPOSITORY  https://github.com/lz4/lz4.git
    GIT_TAG         v1.9.4
    SOURCE_SUBDIR   build/cmake
)

set(lib-md5
        thirdparty/md5/md5.c
        thirdparty/md5/md5.h
)
set(lib-mikktspace
        thirdparty/mikktspace/mikktspace.c
        thirdparty/mikktspace/mikktspace.h
)
set(lib-ogg
        thirdparty/ogg/include/ogg/ogg.h
        thirdparty/ogg/include/ogg/os_types.h
        thirdparty/ogg/src/bitwise.c
        thirdparty/ogg/src/crctable.h
        thirdparty/ogg/src/framing.c
)
set(lib-revorb
        thirdparty/revorb/revorb.cpp
        thirdparty/revorb/revorb.h
)
set(lib-vorbis
        thirdparty/vorbis/include/vorbis/codec.h
        thirdparty/vorbis/include/vorbis/vorbisfile.h
        thirdparty/vorbis/lib/mdct.c
        thirdparty/vorbis/lib/smallft.c
        thirdparty/vorbis/lib/block.c
        thirdparty/vorbis/lib/envelope.c
        thirdparty/vorbis/lib/window.c
        thirdparty/vorbis/lib/lsp.c
        thirdparty/vorbis/lib/lpc.c
        thirdparty/vorbis/lib/analysis.c
        thirdparty/vorbis/lib/synthesis.c
        thirdparty/vorbis/lib/psy.c
        thirdparty/vorbis/lib/info.c
        thirdparty/vorbis/lib/floor1.c
        thirdparty/vorbis/lib/floor0.c
        thirdparty/vorbis/lib/res0.c
        thirdparty/vorbis/lib/mapping0.c
        thirdparty/vorbis/lib/registry.c
        thirdparty/vorbis/lib/codebook.c
        thirdparty/vorbis/lib/sharedbook.c
        thirdparty/vorbis/lib/lookup.c
        thirdparty/vorbis/lib/bitrate.c
        thirdparty/vorbis/lib/vorbisfile.c
)
set(lib-ww2ogg
        thirdparty/ww2ogg/Bit_stream.h
        thirdparty/ww2ogg/crc.c
        thirdparty/ww2ogg/crc.h
        thirdparty/ww2ogg/errors.h
        thirdparty/ww2ogg/packed_codebooks_aoTuV_603.h
        thirdparty/ww2ogg/ww2oggcodebook.cpp
        thirdparty/ww2ogg/ww2oggcodebook.h
        thirdparty/ww2ogg/wwriff.cpp
        thirdparty/ww2ogg/wwriff.h
)

# yyjson
FetchContent_Declare(yyjson
    GIT_REPOSITORY  https://github.com/ibireme/yyjson.git
    GIT_TAG         0.7.0
)

# zhmtools/resourcelib
set(ZHM_BUILD_TOOLS OFF)
FetchContent_Declare(zhmtools
    GIT_REPOSITORY  https://github.com/OrfeasZ/ZHMTools.git
    GIT_TAG         v3.4.1
)

set(lib-quickentity-ffi
        thirdparty/quickentity-ffi/quickentity_ffi.dll.h
)
set(thirdparty
        ${lib-md5}
        ${lib-mikktspace}
        ${lib-ogg}
        ${lib-revorb}
        ${lib-vorbis}
        ${lib-ww2ogg}
        ${lib-quickentity-ffi}
)

# TonyTools
set(TONYTOOLS_BUILD_TOOLS OFF)
FetchContent_Declare(tonytools
    GIT_REPOSITORY  https://github.com/AnthonyFuller/TonyTools.git
    GIT_TAG         v1.8.2
)

FetchContent_MakeAvailable(rjson tonytools gltfsdk directxtex lz4 yyjson json zhmtools directxmath)

configure_file(src/version.h.in src/version.h)

set(RPKG_COMMON_SOURCES
        src/asva.cpp
        src/asva.h
        src/borg.cpp
        src/borg.h
        src/compute_ioi_hash.cpp
        src/compute_ioi_hash_from_file.cpp
        src/console.cpp
        src/crc32.h
        src/crypto.cpp
        src/decrypt_packagedefinition_thumbs.cpp
        src/dev_diff_rpkg_supermetas.cpp
        src/dev_dlge_names.cpp
        src/dev_extract_all_entities_to_qn.cpp
        src/dev_extract_rpkg_supermetas.cpp
        src/dev_extract_wwem_strings.cpp
        src/dev_extract_wwes_strings.cpp
        src/dev_extract_wwev_strings.cpp
        src/dev_extract_wwise_ids.cpp
        src/dev_function.h
        src/hash_list_percent_found.cpp
        src/dev_map_editor.cpp
        src/encrypt_packagedefinition_thumbs.cpp
        src/entity.cpp
        src/extract_all_hash_depends_from.cpp
        src/extract_all_prim_from.cpp
        src/extract_all_prim_model_from.cpp
        src/extract_all_prim_model_of_temp_from.cpp
        src/extract_all_prim_of_temp_from.cpp
        src/extract_all_text_from.cpp
        src/extract_asva_to_json.cpp
        src/extract_direct_hash_depends_from.cpp
        src/extract_dlge_to_json_from.cpp
        src/extract_entity_to_qn.cpp
        src/extract_from_rpkg.cpp
        src/extract_from_rpkg_with_map.cpp
        src/extract_from_rpkgs.cpp
        src/extract_gfxf_from.cpp
        src/extract_hash_meta.cpp
        src/extract_latest_hash.cpp
        src/extract_locr_to_json_from.cpp
        src/extract_all_materials_to_json.cpp
        src/extract_material_to_json.cpp
        src/extract_mati_to_json.cpp
        src/extract_mrtr_to_json.cpp
        src/extract_ores_from.cpp
        src/extract_prel_from.cpp
        src/extract_prim_from.cpp
        src/extract_prim_model_from.cpp
        src/extract_prim_textured_from.cpp
        src/extract_rpkg_meta.cpp
        src/extract_rtlv_to_json_from.cpp
        src/extract_sdef_to_json.cpp
        src/extract_text_from.cpp
        src/extract_to_rt_json.cpp
        src/extract_wwem_to_ogg_from.cpp
        src/extract_wwes_to_ogg_from.cpp
        src/extract_wwev_to_ogg_from.cpp
        src/file.cpp
        src/file_reader.hpp
        src/generate_ioi_treeview.cpp
        src/generate_rpkg_from.cpp
        src/generate_hash_meta_json.cpp
        src/get_latest_hash.cpp
        src/get_line_string.cpp
        src/get_unique_name.cpp
        src/global.cpp
        src/gltf.cpp
        src/gltf.h
        src/gpudevice.cpp
        src/gpudevice.h
        src/hash.cpp
        src/hash_depends.cpp
        src/hash_meta_to_json.cpp
        src/hash_probe.cpp
        src/import_hash_meta.cpp
        src/import_hash_meta_json.cpp
        src/import_rpkg.cpp
        src/import_rpkg_files_in_folder.cpp
        src/import_rpkg_meta.cpp
        src/ioi_treeview.cpp
        src/ioi_treeview.h
        src/json_to_asva.cpp
        src/json_to_hash_meta.cpp
        src/json_to_material.cpp
        src/json_to_mati.cpp
        src/json_to_mrtr.cpp
        src/json_to_sdef.cpp
        src/latest_hash.cpp
        src/load_master_hash_list.cpp
        src/load_temp_tblu_hash_depends.cpp
        src/map.cpp
        src/map.h
        src/material.cpp
        src/material.h
        src/mati.cpp
        src/mati.h
        src/mati_to_json.cpp
        src/mrtr.cpp
        src/mrtr.h
        src/mrtr_to_json.cpp
        src/obj.cpp
        src/obj.h
        src/ores.cpp
        src/pch.cpp
        src/pch.h
        src/prim.cpp
        src/prim.h
        src/rebuild_dlge_from_json_from.cpp
        src/rebuild_gfxf_in.cpp
        src/rebuild_locr_from_json_from.cpp
        src/rebuild_prim_in.cpp
        src/rebuild_prim_model_in.cpp
        src/rebuild_rtlv_from_json_from.cpp
        src/rebuild_text.cpp
        src/rebuild_text_in.cpp
        src/rebuild_wwev_in.cpp
        src/recursive_temp_loader.cpp
        src/repo.cpp
        src/rpkg.cpp
        src/sdef.cpp
        src/sdef.h
        src/sdef_to_json.cpp
        src/search_entities.cpp
        src/search_hash_data.cpp
        src/search_localization.cpp
        src/search_repo.cpp
        src/stream_reader.hpp
        src/stream_writer.hpp
        src/task.cpp
        src/temp.cpp
        src/texd.cpp
        src/text.cpp
        src/util.cpp
)

set(RPKG_LIB_SOURCES
        src/lib/rpkg_dll.cpp
        src/lib/rpkg_dll.h
        )

set(RPKG_CLI_SOURCES
        src/cli/main.cpp
        src/cli/command_line.cpp
        src/cli/command_line.h
        )

add_library(rpkg-lib SHARED ${RPKG_LIB_SOURCES} ${RPKG_COMMON_SOURCES} ${thirdparty})
add_executable(rpkg-cli ${RPKG_CLI_SOURCES} ${RPKG_COMMON_SOURCES} ${thirdparty})

target_compile_definitions(rpkg-lib PUBLIC RPKG_DLL)
target_compile_definitions(rpkg-cli PUBLIC RPKG_CLI)

if (MSVC)
    # C4068 - no unknown pragma
    target_compile_options(rpkg-lib PUBLIC "/bigobj" "/W3" "/wd4068")
    target_compile_options(rpkg-cli PUBLIC "/bigobj" "/W3" "/wd4068")
endif()

target_link_directories(rpkg-cli PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/quickentity-ffi)
target_include_directories(rpkg-lib PUBLIC rapidjson)
target_link_libraries(rpkg-cli PUBLIC
    GLTFSDK
    DirectXTex
    DirectXMath
    lz4_static
    yyjson
    nlohmann_json::nlohmann_json
    ResourceLib_HM2
    ResourceLib_HM2016
    ResourceLib_HM3
    quickentity_ffi.dll.lib
    TonyTools::HMLanguages
)

target_link_directories(rpkg-lib PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/quickentity-ffi)
target_include_directories(rpkg-lib PUBLIC rapidjson)
target_link_libraries(rpkg-lib PUBLIC
    GLTFSDK
    DirectXTex
    DirectXMath
    lz4_static
    yyjson
    nlohmann_json::nlohmann_json
    ResourceLib_HM2
    ResourceLib_HM2016
    ResourceLib_HM3
    quickentity_ffi.dll.lib
    TonyTools::HMLanguages
)

include_directories(${CMAKE_SOURCE_DIR})
# Pre-processed version header file (on configure step, before build)
include_directories(${CMAKE_BINARY_DIR}/src)
