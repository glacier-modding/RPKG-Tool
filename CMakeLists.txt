cmake_minimum_required(VERSION 3.22)
project(rpkg VERSION 2.26.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(thirdparty/GLTFSDK)
add_subdirectory(thirdparty/directxtex)

option(WITH_DIRECTX_SUPPORT
        "If DirectX components and support for .PRIM, .TEXT, and .BORG should be compiled. Currently only works on Windows."
        ON
)

if (WITH_DIRECTX_SUPPORT)
    set(lib-directxmath
            thirdparty/directxmath/DirectXCollision.h
            thirdparty/directxmath/DirectXCollision.inl
            thirdparty/directxmath/DirectXColors.h
            thirdparty/directxmath/DirectXMath.h
            thirdparty/directxmath/DirectXMathConvert.inl
            thirdparty/directxmath/DirectXMathMatrix.inl
            thirdparty/directxmath/DirectXMathMisc.inl
            thirdparty/directxmath/DirectXMathVector.inl
            thirdparty/directxmath/DirectXPackedVector.h
            thirdparty/directxmath/DirectXPackedVector.inl
    )
endif()
set(lib-json
        thirdparty/json/json.hpp
)
set(lib-lz4
        thirdparty/lz4/LICENSE
        thirdparty/lz4/lz4.c
        thirdparty/lz4/lz4.h
        thirdparty/lz4/lz4frame.c
        thirdparty/lz4/lz4frame.h
        thirdparty/lz4/lz4hc.c
        thirdparty/lz4/lz4hc.h
        thirdparty/lz4/xxhash.c
        thirdparty/lz4/xxhash.h
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
set(lib-rapidjson
        thirdparty/rapidjson/error/en.h
        thirdparty/rapidjson/error/error.h
        thirdparty/rapidjson/internal/biginteger.h
        thirdparty/rapidjson/internal/clzll.h
        thirdparty/rapidjson/internal/diyfp.h
        thirdparty/rapidjson/internal/dtoa.h
        thirdparty/rapidjson/internal/ieee754.h
        thirdparty/rapidjson/internal/itoa.h
        thirdparty/rapidjson/internal/meta.h
        thirdparty/rapidjson/internal/pow10.h
        thirdparty/rapidjson/internal/regex.h
        thirdparty/rapidjson/internal/stack.h
        thirdparty/rapidjson/internal/strfunc.h
        thirdparty/rapidjson/internal/strtod.h
        thirdparty/rapidjson/internal/swap.h
        thirdparty/rapidjson/msinttypes/inttypes.h
        thirdparty/rapidjson/msinttypes/stdint.h
        thirdparty/rapidjson/allocators.h
        thirdparty/rapidjson/cursorstreamwrapper.h
        thirdparty/rapidjson/document.h
        thirdparty/rapidjson/encodedstream.h
        thirdparty/rapidjson/encodings.h
        thirdparty/rapidjson/filereadstream.h
        thirdparty/rapidjson/filewritestream.h
        thirdparty/rapidjson/fwd.h
        thirdparty/rapidjson/istreamwrapper.h
        thirdparty/rapidjson/memorybuffer.h
        thirdparty/rapidjson/memorystream.h
        thirdparty/rapidjson/ostreamwrapper.h
        thirdparty/rapidjson/pointer.h
        thirdparty/rapidjson/prettywriter.h
        thirdparty/rapidjson/rapidjson.h
        thirdparty/rapidjson/reader.h
        thirdparty/rapidjson/schema.h
        thirdparty/rapidjson/stream.h
        thirdparty/rapidjson/stringbuffer.h
        thirdparty/rapidjson/writer.h
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
set(lib-yyjson
        thirdparty/yyjson/yyjson.c
        thirdparty/yyjson/yyjson.h
)
set(lib-zhmtools
        thirdparty/zhmtools/Embind.h
        thirdparty/zhmtools/ResourceConverter.h
        thirdparty/zhmtools/ResourceGenerator.h
        thirdparty/zhmtools/ResourceLib.h
        thirdparty/zhmtools/ResourceLib_HM2.h
        thirdparty/zhmtools/ResourceLib_HM3.h
        thirdparty/zhmtools/ResourceLib_HM2016.h
        thirdparty/zhmtools/ResourceLibCommon.h
)
set(thirdparty
#        ${lib-directxmath}
        ${lib-json}
        ${lib-lz4}
        ${lib-md5}
        ${lib-mikktspace}
        ${lib-ogg}
        ${lib-rapidjson}
        ${lib-revorb}
        ${lib-vorbis}
        ${lib-ww2ogg}
        ${lib-yyjson}
        ${lib-zhmtools}
)

configure_file(src/version.h.in src/version.h)

set(RPKG_COMMON_SOURCES
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
        src/dev_extract_all_strings.cpp
        src/dev_extract_rpkg_supermetas.cpp
        src/dev_extract_wwem_strings.cpp
        src/dev_extract_wwes_strings.cpp
        src/dev_extract_wwev_strings.cpp
        src/dev_extract_wwise_ids.cpp
        src/dev_function.h
        src/dev_hash_list_percent_found.cpp
        src/dev_map_editor.cpp
        src/encrypt_packagedefinition_thumbs.cpp
        src/entity.cpp
        src/extract_all_hash_depends_from.cpp
        src/extract_all_prim_from.cpp
        src/extract_all_prim_model_from.cpp
        src/extract_all_prim_model_of_temp_from.cpp
        src/extract_all_prim_of_temp_from.cpp
        src/extract_all_text_from.cpp
        src/extract_direct_hash_depends_from.cpp
        src/extract_dlge_to_json_from.cpp
        src/extract_entity_to_qn.cpp
        src/extract_from_rpkg.cpp
        src/extract_from_rpkg_with_map.cpp
        src/extract_from_rpkgs.cpp
        src/extract_gfxf_from.cpp
        src/extract_hash_meta.cpp
        src/extract_locr_to_json_from.cpp
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
        src/extract_temp_from.cpp
        src/extract_text_from.cpp
        src/extract_to_rt_json.cpp
        src/extract_wwem_to_ogg_from.cpp
        src/extract_wwes_to_ogg_from.cpp
        src/extract_wwev_to_ogg_from.cpp
        src/file.cpp
        src/file_reader.hpp
        src/generate_rpkg_from.cpp
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
        src/import_rpkg.cpp
        src/import_rpkg_files_in_folder.cpp
        src/import_rpkg_meta.cpp
        src/json_to_hash_meta.cpp
        src/json_to_mati.cpp
        src/json_to_mrtr.cpp
        src/json_to_sdef.cpp
        src/latest_hash.cpp
        src/load_master_hash_list.cpp
        src/load_temp_tblu_hash_depends.cpp
        src/map.cpp
        src/map.h
        src/mati.cpp
        src/mati.h
        src/mati_to_json.cpp
        src/mrtr.cpp
        src/mrtr.h
        src/mrtr_to_json.cpp
        src/obj.cpp
        src/obj.h
        src/ores.cpp
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
    target_compile_options(rpkg-lib PUBLIC "/bigobj" "/W3" "/wd4068" "/Gy")
    target_compile_options(rpkg-cli PUBLIC "/bigobj" "/W3" "/wd4068" "/Gy")
endif()

target_link_directories(rpkg-cli PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/zhmtools)
target_link_libraries(rpkg-cli PUBLIC GLTFSDK DirectXTex ResourceLib_HM2 ResourceLib_HM2016 ResourceLib_HM3)

target_link_directories(rpkg-lib PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/zhmtools)
target_link_libraries(rpkg-lib PUBLIC GLTFSDK DirectXTex ResourceLib_HM2 ResourceLib_HM2016 ResourceLib_HM3)

include_directories(${CMAKE_SOURCE_DIR})
# Pre-processed version header file (on configure step, before build)
include_directories(${CMAKE_BINARY_DIR}/src)
