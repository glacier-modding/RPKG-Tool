#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include "prim.h"
#include "thirdparty/json/json.hpp"
#include "thirdparty/mikktspace/mikktspace.h"
#include "thirdparty/directxmath/DirectXMath.h"
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

using json = nlohmann::ordered_json;

using namespace Microsoft::glTF;

using namespace DirectX;

class StreamReader final : public IStreamReader {
public:
    explicit StreamReader(std::filesystem::path pathBase) : m_pathBase(std::move(pathBase)) {
        assert(m_pathBase.has_root_path());
    }

    [[nodiscard]] std::shared_ptr<std::istream> GetInputStream(const std::string& filename) const override {
        auto streamPath = m_pathBase / std::filesystem::u8path(filename);
        auto stream = std::make_shared<std::ifstream>(streamPath, std::ios_base::binary);

        if (!stream || !(*stream)) {
            throw std::runtime_error("Unable to create a valid input stream for uri: " + filename);
        }

        return stream;
    }

private:
    std::filesystem::path m_pathBase;
};

class MikkTSpace_Data {
public:
    const std::vector<uint16_t>& indices;
    const std::vector<float>& vertices;
    const std::vector<float>& normals;
    std::vector<float>& tangents;
    std::vector<float>& bitangents;
    const std::vector<float>& uvs;

    MikkTSpace_Data(
            const std::vector<uint16_t>& input_indices,
            const std::vector<float>& input_vertices,
            const std::vector<float>& input_normals,
            std::vector<float>& input_tangents,
            std::vector<float>& input_bitangents,
            const std::vector<float>& input_uvs
    )
            :
            indices(input_indices),
            vertices(input_vertices),
            normals(input_normals),
            tangents(input_tangents),
            bitangents(input_bitangents),
            uvs(input_uvs) {
    }
};

static int mikktspace_getNumFaces(const SMikkTSpaceContext* Context) {
    const MikkTSpace_Data* input_data = static_cast<MikkTSpace_Data*>(Context->m_pUserData);

    return (input_data->indices.size() / 3);
}

static int mikktspace_getNumVerticesOfFace(const SMikkTSpaceContext* Context, const int FaceIdx) {
    return 3;
}

static void
mikktspace_getPosition(const SMikkTSpaceContext* Context, float Position[3], const int FaceIdx, const int VertIdx) {
    const MikkTSpace_Data* input_data = static_cast<MikkTSpace_Data*>(Context->m_pUserData);

    Position[0] = input_data->vertices[input_data->indices[FaceIdx * 3 + VertIdx] * 3];
    Position[1] = input_data->vertices[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 1];
    Position[2] = input_data->vertices[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 2];
}

static void
mikktspace_getNormal(const SMikkTSpaceContext* Context, float Normal[3], const int FaceIdx, const int VertIdx) {
    const MikkTSpace_Data* input_data = static_cast<MikkTSpace_Data*>(Context->m_pUserData);

    Normal[0] = input_data->normals[input_data->indices[FaceIdx * 3 + VertIdx] * 3];
    Normal[1] = input_data->normals[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 1];
    Normal[2] = input_data->normals[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 2];
}

static void
mikktspace_setTSpaceBasic(const SMikkTSpaceContext* Context, const float Tangent[3], const float BitangentSign,
                          const int FaceIdx, const int VertIdx) {
    const MikkTSpace_Data* input_data = static_cast<MikkTSpace_Data*>(Context->m_pUserData);

    input_data->tangents[input_data->indices[FaceIdx * 3 + VertIdx] * 3] = Tangent[0];
    input_data->tangents[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 1] = Tangent[1];
    input_data->tangents[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 2] = Tangent[2];

    DirectX::XMVECTOR tangent_vector;
    tangent_vector.m128_f32[0] = Tangent[0];
    tangent_vector.m128_f32[1] = Tangent[1];
    tangent_vector.m128_f32[2] = Tangent[2];

    DirectX::XMVECTOR normal_vector;
    normal_vector.m128_f32[0] = input_data->normals[input_data->indices[FaceIdx * 3 + VertIdx] * 3];
    normal_vector.m128_f32[1] = input_data->normals[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 1];
    normal_vector.m128_f32[2] = input_data->normals[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 2];

    DirectX::XMVECTOR bitangent_vector = BitangentSign * DirectX::XMVector3Cross(normal_vector, tangent_vector);

    input_data->bitangents[input_data->indices[FaceIdx * 3 + VertIdx] * 3] = bitangent_vector.m128_f32[0];
    input_data->bitangents[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 1] = bitangent_vector.m128_f32[1];
    input_data->bitangents[input_data->indices[FaceIdx * 3 + VertIdx] * 3 + 2] = bitangent_vector.m128_f32[2];
}

static void
mikktspace_getTexCoord(const SMikkTSpaceContext* Context, float UV[2], const int FaceIdx, const int VertIdx) {
    const MikkTSpace_Data* input_data = static_cast<MikkTSpace_Data*>(Context->m_pUserData);

    UV[0] = input_data->uvs[input_data->indices[FaceIdx * 3 + VertIdx] * 2];
    UV[1] = input_data->uvs[input_data->indices[FaceIdx * 3 + VertIdx] * 2 + 1];
}

void rpkg_function::rebuild_prim_in(std::string& input_path, bool generate_rpkgs) {
    task_single_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to rebuild the PRIMs does not exist.");
    }

    std::vector<std::string> prim_folders;

    std::string input_folder = input_rpkg_folder_path;

    std::string rpkgs_path = input_rpkg_folder_path + "RPKGS";

    if (generate_rpkgs) {
        file::create_directories(rpkgs_path);
    }

    std::filesystem::path input_folder_path = input_rpkg_folder_path;

    if (input_folder.substr(input_folder.length() - 1, 1) == "\\") {
        input_folder = input_folder.substr(0, input_folder.length() - 1);
    }

    prim_folders.push_back(input_folder);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path)) {
        if (!std::filesystem::is_directory(entry.path().string()))
            continue;

        input_folder = entry.path().string();

        if (input_folder.substr(input_folder.length() - 1, 1) == "\\") {
            input_folder = input_folder.substr(0, input_folder.length() - 1);
        }

        bool prim_folder_found = false;

        for (const auto& prim_folder : prim_folders) {
            if (input_folder == prim_folder) {
                prim_folder_found = true;
            }
        }

        if (!prim_folder_found) {
            prim_folders.push_back(input_folder);
        }
    }

    LOG("Scanning folder: Done");
    LOG("Rebuilding PRIM files...");

    initialize_prim_float_values();

    std::set<std::string> directory_set;

    for (uint64_t i = 0; i < prim_folders.size(); i++) {
        if (gui_control == ABORT_CURRENT_TASK) {
            return;
        }

        std::vector<std::string> glb_file_names;
        std::vector<std::string> glb_file_paths;
        std::vector<std::string> glb_meta_file_paths;
        std::vector<uint64_t> prim_hashes;
        std::vector<std::string> prim_hash_strings;
        std::vector<std::string> prim_file_names;

        for (const auto& entry : std::filesystem::directory_iterator(prim_folders.at(i))) {
            if (!std::filesystem::is_regular_file(entry.path().string()))
                continue;

            std::size_t pos = entry.path().string().find_last_of("\\/");

            std::string file_name = "";
            std::string hash_file_name = "";
            std::string hash_string = "";
            std::string resource_type = "";

            if (pos != std::string::npos) {
                file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
            } else {
                file_name = entry.path().string();
            }

            if (file_name.length() <= 4 || util::to_upper_case(file_name.substr(file_name.length() - 4, 4)) != ".GLB")
                continue;

            hash_file_name = util::to_upper_case(file_name.substr(0, (file_name.length() - 4)));

            std::string meta_file_path = file::output_path_append("metas\\" + hash_file_name + ".glb.meta",
                                                                  prim_folders.at(i));

            pos = hash_file_name.find_last_of('.');

            if (pos != std::string::npos) {
                hash_string = hash_file_name.substr(0, pos);
                resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
            }

            bool is_glb_hash_file = true;
            bool glb_hash_meta = true;

            if (hash_string.length() != 16) {
                is_glb_hash_file = false;
            }

            if (resource_type != "PRIM") {
                is_glb_hash_file = false;
            }

            if (!file::path_exists(meta_file_path)) {
                glb_hash_meta = false;

                LOG("GLB file " + entry.path().string() + " found but meta file is missing, can't rebuild.");

                task_status_string =
                        "GLB file " + entry.path().string() + " found but meta file is missing, can't rebuild.";

                task_multiple_status = PRIM_REBUILD_META_FILE_MISSING;

                return;
            }

            if (is_glb_hash_file && glb_hash_meta) {
                glb_file_paths.push_back(entry.path().string());
                glb_file_names.push_back(file_name);
                glb_meta_file_paths.push_back(meta_file_path);
                prim_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                prim_file_names.push_back(util::to_upper_case(hash_file_name));
                prim_hash_strings.push_back(util::to_upper_case(hash_string));
            }
        }

        for (uint64_t j = 0; j < glb_file_paths.size(); j++) {
            std::vector<char> prim_file_data(0x10, 0x0);
            std::vector<char> prim_file_header_data;
            std::vector<uint32_t> prim_file_mesh_object_offsets;

            std::filesystem::path path = glb_file_paths.at(j);

            auto streamReader = std::make_unique<StreamReader>(path.parent_path());

            std::filesystem::path pathFile = path.filename();

            std::string manifest;

            std::unique_ptr<GLTFResourceReader> resourceReader;

            auto glbStream = streamReader->GetInputStream(pathFile.u8string());
            auto glbResourceReader = std::make_unique<GLBResourceReader>(std::move(streamReader), std::move(glbStream));

            manifest = glbResourceReader->GetJson();

            resourceReader = std::move(glbResourceReader);

            if (!resourceReader) {
                LOG_AND_EXIT("Command line argument path filename extension must be .gltf or .glb");
            }

            Document document;

            try {
                document = Deserialize(manifest);
            }
            catch (const GLTFException& ex) {
                std::stringstream ss;

                ss << "Microsoft::glTF::Deserialize failed: ";
                ss << ex.what();

                LOG_AND_EXIT(ss.str());
            }

            LOG_NO_ENDL("### glTF Info - " + pathFile.generic_string() + " ###\n\n");

            LOG_NO_ENDL("Asset Version:    " + document.asset.version + "\n");
            LOG_NO_ENDL("Asset MinVersion: " + document.asset.minVersion + "\n");
            LOG_NO_ENDL("Asset Generator:  " + document.asset.generator + "\n");
            LOG_NO_ENDL("Asset Copyright:  " + document.asset.copyright + "\n\n");

            LOG_NO_ENDL("Scene Count: " + util::uint32_t_to_string(document.scenes.Size()) + "\n");

            if (document.scenes.Size() > 0U) {
                LOG_NO_ENDL("Default Scene Index: " + document.GetDefaultScene().id + "\n\n");
            } else {
                LOG_NO_ENDL("\n");
            }

            LOG_NO_ENDL("Node Count:     " + util::uint32_t_to_string(document.nodes.Size()) + "\n");
            LOG_NO_ENDL("Camera Count:   " + util::uint32_t_to_string(document.cameras.Size()) + "\n");
            LOG_NO_ENDL("Material Count: " + util::uint32_t_to_string(document.materials.Size()) + "\n\n");

            LOG_NO_ENDL("Mesh Count: " + util::uint32_t_to_string(document.meshes.Size()) + "\n");
            LOG_NO_ENDL("Skin Count: " + util::uint32_t_to_string(document.skins.Size()) + "\n\n");

            LOG_NO_ENDL("Image Count:   " + util::uint32_t_to_string(document.images.Size()) + "\n");
            LOG_NO_ENDL("Texture Count: " + util::uint32_t_to_string(document.textures.Size()) + "\n");
            LOG_NO_ENDL("Sampler Count: " + util::uint32_t_to_string(document.samplers.Size()) + "\n\n");

            LOG_NO_ENDL("Buffer Count:     " + util::uint32_t_to_string(document.buffers.Size()) + "\n");
            LOG_NO_ENDL("BufferView Count: " + util::uint32_t_to_string(document.bufferViews.Size()) + "\n");
            LOG_NO_ENDL("Accessor Count:   " + util::uint32_t_to_string(document.accessors.Size()) + "\n\n");

            LOG_NO_ENDL("Animation Count: " + util::uint32_t_to_string(document.animations.Size()) + "\n\n");

            for (const auto& extension : document.extensionsUsed) {
                LOG_NO_ENDL("Extension Used: " + extension + "\n");
            }

            if (!document.extensionsUsed.empty()) {
                LOG_NO_ENDL("\n");
            }

            for (const auto& extension : document.extensionsRequired) {
                LOG_NO_ENDL("Extension Required: " + extension + "\n");
            }

            if (!document.extensionsRequired.empty()) {
                LOG_NO_ENDL("\n");
            }

            prim temp_prim(glb_meta_file_paths.at(j));

            LOG("PRIM meta object offsets size: " + util::uint32_t_to_string(temp_prim.prim_object_offsets.size()));

            bool lods_match = false;

            uint32_t glb_mesh_count = 0;

            std::vector<uint32_t> prim_object_to_node_index(temp_prim.prim_object_offsets.size(), 0);
            std::vector<std::string> prim_object_to_mesh_index(temp_prim.prim_object_offsets.size(), "");

            for (uint32_t n = 0; n < document.nodes.Size(); n++) {
                Node temp_node = document.nodes.Get(n);

                if (!temp_node.meshId.empty()) {
                    Mesh temp_mesh = document.meshes.Get(temp_node.meshId);

                    size_t pos = temp_mesh.name.find_last_of('_');

                    if (pos != std::string::npos) {
                        std::string mesh_value = temp_mesh.name.substr(pos + 1);

                        bool is_all_digits = true;

                        for (char l : mesh_value) {
                            if (!std::isdigit(l)) {
                                is_all_digits = false;
                            }
                        }

                        uint32_t mesh_number = std::strtoul(mesh_value.c_str(), nullptr, 10);

                        if (is_all_digits && mesh_number < temp_prim.prim_object_offsets.size()) {
                            prim_object_to_node_index.at(mesh_number) = n;
                            prim_object_to_mesh_index.at(mesh_number) = temp_mesh.id;
                        } else {
                            LOG("Error: " + glb_file_names.at(j) + "'s mesh name " + temp_mesh.name +
                                " value (0 indexed) " + mesh_value + " exceeds the original mesh count (" +
                                util::uint32_t_to_string(temp_prim.prim_object_offsets.size()) + ") in the PRIM.");

                            task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh name " + temp_mesh.name +
                                                 " value (0 indexed) " + mesh_value +
                                                 " exceeds the original mesh count (" +
                                                 util::uint32_t_to_string(temp_prim.prim_object_offsets.size()) +
                                                 ") in the PRIM.";

                            task_multiple_status = PRIM_REBUILD_GLB_MESH_NAME_MALFORMED;

                            return;
                        }
                    } else {
                        LOG("Error: " + glb_file_names.at(j) + "'s mesh name " + temp_mesh.name + " is malformed.");

                        task_status_string =
                                "Error: " + glb_file_names.at(j) + "'s mesh name " + temp_mesh.name + " is malformed.";

                        task_multiple_status = PRIM_REBUILD_GLB_MESH_NAME_MALFORMED;

                        return;
                    }

                    glb_mesh_count++;
                }
            }

            if (temp_prim.prim_object_offsets.size() == glb_mesh_count) {
                lods_match = true;
            }

            if (lods_match) {
                LOG("LODs match.");
            } else {
                LOG("LODs do not match.");
            }

            LOG("GLB object / node size: " << glb_mesh_count);

            std::vector<float> vertices_min_all;
            std::vector<float> vertices_max_all;

            bool first_mesh = true;

            uint32_t object_count_total = 0;

            for (uint32_t m = 0; m < prim_object_to_mesh_index.size(); m++) {
                if (!prim_object_to_mesh_index.at(m).empty()) {
                    object_count_total++;

                    Node temp_node = document.nodes.Get(prim_object_to_node_index.at(m));

                    Mesh temp_mesh = document.meshes.Get(prim_object_to_mesh_index.at(m));

                    LOG_NO_ENDL("Node: " + temp_node.id + "\n");

                    LOG_NO_ENDL("Mesh: " + temp_mesh.id + "\n");

                    if (temp_mesh.primitives.size() > 1) {
                        LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                            " has more than one mesh primitive, only one mesh primitive per mesh is allowed.");

                        task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                             " has more than one mesh primitive, only one mesh primitive per mesh is allowed.";

                        task_multiple_status = PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED;

                        return;
                    }

                    for (const auto& meshPrimitive : temp_mesh.primitives) {
                        char char1;
                        char char2[2];
                        char char4[4];

                        std::string accessorId;
                        std::vector<uint16_t> indices_data;
                        // uint32_t indices_data_size = 0;
                        std::vector<float> vertices_data;
                        uint32_t vertices_data_size = 0;
                        std::vector<float> normals_data;
                        uint32_t normals_data_size = 0;
                        std::vector<float> uvs_data;
                        uint32_t uvs_data_size = 0;
                        std::vector<uint8_t> colors_data_uint8_t;
                        uint32_t colors_data_size_uint8_t = 0;
                        std::vector<uint16_t> colors_data_uint16_t;
                        uint32_t colors_data_size_uint16_t = 0;
                        std::vector<uint8_t> joints_0_data;
                        uint32_t joints_0_data_size = 0;
                        std::vector<float> weights_0_data;
                        uint32_t weights_0_data_size = 0;
                        std::vector<uint8_t> joints_1_data;
                        uint32_t joints_1_data_size = 0;
                        std::vector<float> weights_1_data;
                        uint32_t weights_1_data_size = 0;
                        std::vector<float> temp_vertexes_min;
                        std::vector<float> temp_vertexes_max;
                        std::vector<float> temp_uvs_min;
                        std::vector<float> temp_uvs_max;
                        std::vector<float> vertices_compression_scale(3, 0);
                        std::vector<float> vertices_compression_bias(3, 0);
                        std::vector<float> uvs_compression_scale(2, 0);
                        std::vector<float> uvs_compression_bias(2, 0);
                        // uint32_t offset_mesh = 0;
                        uint32_t offset_sub_mesh = 0;
                        uint32_t offset_sub_mesh_pointer = 0;
                        uint32_t offset_indices = 0;
                        uint32_t offset_vertices = 0;
                        uint32_t offset_collision = 0;
                        uint32_t offset_cloth = 0;
                        uint32_t offset_bones_nodes = 0;
                        uint32_t offset_bones_indices = 0;
                        uint32_t offset_bones_info = 0;

                        uint32_t indices_count = 0;
                        uint32_t vertex_count = 0;

                        offset_indices = prim_file_data.size();

                        {
                            const Accessor& accessor_indices = document.accessors.Get(meshPrimitive.indicesAccessorId);

                            indices_data = resourceReader->ReadBinaryData<uint16_t>(document, accessor_indices);
                            uint32_t indices_data_size = indices_data.size() * sizeof(uint16_t);

                            LOG_NO_ENDL("MeshPrimitive: " + util::uint32_t_to_string(indices_data_size) +
                                        " bytes of index data\n");

                            indices_count = indices_data.size();

                            for (unsigned short& a : indices_data) {
                                std::memcpy(&char2, &a, sizeof(uint16_t));

                                for (char b : char2) {
                                    prim_file_data.push_back(b);
                                }
                            }

                            while (prim_file_data.size() % 0x10 != 0) {
                                prim_file_data.push_back(0x0);
                            }
                        }

                        offset_vertices = prim_file_data.size();

                        if (meshPrimitive.TryGetAttributeAccessorId("POSITION", accessorId)) {
                            const Accessor& accessor = document.accessors.Get(accessorId);

                            vertices_data = resourceReader->ReadBinaryData<float>(document, accessor);
                            vertices_data_size = vertices_data.size() * sizeof(float);

                            LOG_NO_ENDL("MeshPrimitive: " + util::uint32_t_to_string(vertices_data_size) +
                                        " bytes of position data\n");

                            if (vertices_data.size() % 3 == 0) {
                                vertex_count = vertices_data.size() / 3;
                            } else {
                                LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                    " vertex float size (" + util::uint32_t_to_string(vertices_data.size()) +
                                    ") is not a multiple of 3.");

                                task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                     " vertex float size (" +
                                                     util::uint32_t_to_string(vertices_data.size()) +
                                                     ") is not a multiple of 3.";

                                task_multiple_status = PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3;

                                return;
                            }

                            if (temp_prim.prim_object_headers.at(0).flag_high_resolution) {
                                for (uint32_t v = 0; v < vertex_count; v++) {
                                    uint32_t x = v * 3;
                                    uint32_t y = v * 3 + 1;
                                    uint32_t z = v * 3 + 2;

                                    if (v == 0) {
                                        temp_vertexes_min.push_back(vertices_data.at(x));
                                        temp_vertexes_min.push_back(vertices_data.at(y));
                                        temp_vertexes_min.push_back(vertices_data.at(z));
                                        temp_vertexes_max.push_back(vertices_data.at(x));
                                        temp_vertexes_max.push_back(vertices_data.at(y));
                                        temp_vertexes_max.push_back(vertices_data.at(z));
                                    } else {
                                        if (vertices_data.at(x) < temp_vertexes_min.at(0))
                                            temp_vertexes_min.at(0) = vertices_data.at(x);
                                        if (vertices_data.at(x) > temp_vertexes_max.at(0))
                                            temp_vertexes_max.at(0) = vertices_data.at(x);
                                        if (vertices_data.at(y) < temp_vertexes_min.at(1))
                                            temp_vertexes_min.at(1) = vertices_data.at(y);
                                        if (vertices_data.at(y) > temp_vertexes_max.at(1))
                                            temp_vertexes_max.at(1) = vertices_data.at(y);
                                        if (vertices_data.at(z) < temp_vertexes_min.at(2))
                                            temp_vertexes_min.at(2) = vertices_data.at(z);
                                        if (vertices_data.at(z) > temp_vertexes_max.at(2))
                                            temp_vertexes_max.at(2) = vertices_data.at(z);
                                    }
                                }

                                if (first_mesh) {
                                    vertices_min_all = temp_vertexes_min;
                                    vertices_max_all = temp_vertexes_max;

                                    first_mesh = false;
                                } else {
                                    if (temp_vertexes_min.at(0) < vertices_min_all.at(0))
                                        vertices_min_all.at(0) = temp_vertexes_min.at(0);
                                    if (temp_vertexes_max.at(0) > vertices_max_all.at(0))
                                        vertices_max_all.at(0) = temp_vertexes_max.at(0);
                                    if (temp_vertexes_min.at(1) < vertices_min_all.at(1))
                                        vertices_min_all.at(1) = temp_vertexes_min.at(1);
                                    if (temp_vertexes_max.at(1) > vertices_max_all.at(1))
                                        vertices_max_all.at(1) = temp_vertexes_max.at(1);
                                    if (temp_vertexes_min.at(2) < vertices_min_all.at(2))
                                        vertices_min_all.at(2) = temp_vertexes_min.at(2);
                                    if (temp_vertexes_max.at(2) > vertices_max_all.at(2))
                                        vertices_max_all.at(2) = temp_vertexes_max.at(2);
                                }

                                for (uint32_t v = 0; v < vertices_compression_scale.size(); v++) {
                                    vertices_compression_scale[v] = 0.5 * (temp_vertexes_max[v] - temp_vertexes_min[v]);
                                }

                                for (uint32_t v = 0; v < vertices_compression_bias.size(); v++) {
                                    vertices_compression_bias[v] = temp_vertexes_min[v] + vertices_compression_scale[v];
                                }

                                for (uint32_t v = 0; v < vertex_count; v++) {
                                    uint32_t x = v * 3;
                                    uint32_t y = v * 3 + 1;
                                    uint32_t z = v * 3 + 2;

                                    std::memcpy(&char4, &vertices_data[x], sizeof(uint32_t));

                                    for (char a : char4) {
                                        prim_file_data.push_back(a);
                                    }

                                    std::memcpy(&char4, &vertices_data[y], sizeof(uint32_t));

                                    for (char a : char4) {
                                        prim_file_data.push_back(a);
                                    }

                                    std::memcpy(&char4, &vertices_data[z], sizeof(uint32_t));

                                    for (char a : char4) {
                                        prim_file_data.push_back(a);
                                    }
                                }
                            } else {
                                for (uint32_t v = 0; v < vertex_count; v++) {
                                    uint32_t x = v * 3;
                                    uint32_t y = v * 3 + 1;
                                    uint32_t z = v * 3 + 2;

                                    if (v == 0) {
                                        temp_vertexes_min.push_back(vertices_data.at(x));
                                        temp_vertexes_min.push_back(vertices_data.at(y));
                                        temp_vertexes_min.push_back(vertices_data.at(z));
                                        temp_vertexes_max.push_back(vertices_data.at(x));
                                        temp_vertexes_max.push_back(vertices_data.at(y));
                                        temp_vertexes_max.push_back(vertices_data.at(z));
                                    } else {
                                        if (vertices_data.at(x) < temp_vertexes_min.at(0))
                                            temp_vertexes_min.at(0) = vertices_data.at(x);
                                        if (vertices_data.at(x) > temp_vertexes_max.at(0))
                                            temp_vertexes_max.at(0) = vertices_data.at(x);
                                        if (vertices_data.at(y) < temp_vertexes_min.at(1))
                                            temp_vertexes_min.at(1) = vertices_data.at(y);
                                        if (vertices_data.at(y) > temp_vertexes_max.at(1))
                                            temp_vertexes_max.at(1) = vertices_data.at(y);
                                        if (vertices_data.at(z) < temp_vertexes_min.at(2))
                                            temp_vertexes_min.at(2) = vertices_data.at(z);
                                        if (vertices_data.at(z) > temp_vertexes_max.at(2))
                                            temp_vertexes_max.at(2) = vertices_data.at(z);
                                    }
                                }

                                if (first_mesh) {
                                    vertices_min_all = temp_vertexes_min;
                                    vertices_max_all = temp_vertexes_max;

                                    first_mesh = false;
                                } else {
                                    if (temp_vertexes_min.at(0) < vertices_min_all.at(0))
                                        vertices_min_all.at(0) = temp_vertexes_min.at(0);
                                    if (temp_vertexes_max.at(0) > vertices_max_all.at(0))
                                        vertices_max_all.at(0) = temp_vertexes_max.at(0);
                                    if (temp_vertexes_min.at(1) < vertices_min_all.at(1))
                                        vertices_min_all.at(1) = temp_vertexes_min.at(1);
                                    if (temp_vertexes_max.at(1) > vertices_max_all.at(1))
                                        vertices_max_all.at(1) = temp_vertexes_max.at(1);
                                    if (temp_vertexes_min.at(2) < vertices_min_all.at(2))
                                        vertices_min_all.at(2) = temp_vertexes_min.at(2);
                                    if (temp_vertexes_max.at(2) > vertices_max_all.at(2))
                                        vertices_max_all.at(2) = temp_vertexes_max.at(2);
                                }

                                for (uint32_t v = 0; v < vertices_compression_scale.size(); v++) {
                                    vertices_compression_scale[v] = 0.5 * (temp_vertexes_max[v] - temp_vertexes_min[v]);
                                }

                                for (uint32_t v = 0; v < vertices_compression_bias.size(); v++) {
                                    vertices_compression_bias[v] = temp_vertexes_min[v] + vertices_compression_scale[v];
                                }

                                for (uint32_t v = 0; v < vertex_count; v++) {
                                    uint32_t x = v * 3;
                                    uint32_t y = v * 3 + 1;
                                    uint32_t z = v * 3 + 2;

                                    auto temp_short = static_cast<short>(std::roundf(
                                            std::numeric_limits<short>::max() *
                                            (vertices_data[x] - vertices_compression_bias[0]) /
                                            vertices_compression_scale[0]));

                                    std::memcpy(&char2, &temp_short, sizeof(uint16_t));

                                    for (char a : char2) {
                                        prim_file_data.push_back(a);
                                    }

                                    temp_short = static_cast<short>(std::roundf(std::numeric_limits<short>::max() *
                                                                                (vertices_data[y] -
                                                                                 vertices_compression_bias[1]) /
                                                                                vertices_compression_scale[1]));

                                    std::memcpy(&char2, &temp_short, sizeof(uint16_t));

                                    for (char a : char2) {
                                        prim_file_data.push_back(a);
                                    }

                                    temp_short = static_cast<short>(std::roundf(std::numeric_limits<short>::max() *
                                                                                (vertices_data[z] -
                                                                                 vertices_compression_bias[2]) /
                                                                                vertices_compression_scale[2]));

                                    std::memcpy(&char2, &temp_short, sizeof(uint16_t));

                                    for (char a : char2) {
                                        prim_file_data.push_back(a);
                                    }

                                    temp_short = 0x7FFF;

                                    std::memcpy(&char2, &temp_short, sizeof(uint16_t));

                                    for (char a : char2) {
                                        prim_file_data.push_back(a);
                                    }
                                }
                            }
                        } else {
                            LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                " is missing POSITION data.");

                            task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                 " is missing POSITION data.";

                            task_multiple_status = PRIM_REBUILD_MISSING_POSITION_DATA;

                            return;
                        }

                        if (temp_prim.prim_object_is_weighted.at(m)) {
                            if (meshPrimitive.HasAttribute("JOINTS_0") && meshPrimitive.HasAttribute("WEIGHTS_0") &&
                                !temp_node.skinId.empty()) {
                                Skin temp_skin = document.skins.Get(temp_node.skinId);

                                std::vector<uint32_t> glb_bone_indexes(temp_skin.jointIds.size());

                                for (uint32_t s = 0; s < temp_skin.jointIds.size(); s++) {
                                    try {
                                        glb_bone_indexes[s] = temp_prim.borg_bone_name_map.at(
                                                document.nodes.Get(temp_skin.jointIds[s]).name);
                                    }
                                    catch (...) {
                                        LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                            " has mismatched bones compared to the original BORG file.");

                                        task_status_string =
                                                "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " has mismatched bones compared to the original BORG file.";

                                        task_multiple_status = PRIM_REBUILD_MISMATCHED_BONES;

                                        return;
                                    }
                                }

                                if (meshPrimitive.TryGetAttributeAccessorId("JOINTS_0", accessorId)) {
                                    const Accessor& accessor = document.accessors.Get(accessorId);

                                    joints_0_data = resourceReader->ReadBinaryData<uint8_t>(document, accessor);
                                    joints_0_data_size = joints_0_data.size() * sizeof(uint8_t);

                                    LOG_NO_ENDL("MeshPrimitive: " + util::uint32_t_to_string(joints_0_data_size) +
                                                " bytes of joints_0 data\n");
                                }

                                if (meshPrimitive.TryGetAttributeAccessorId("WEIGHTS_0", accessorId)) {
                                    const Accessor& accessor = document.accessors.Get(accessorId);

                                    weights_0_data = resourceReader->ReadBinaryData<float>(document, accessor);
                                    weights_0_data_size = weights_0_data.size() * sizeof(float);

                                    LOG_NO_ENDL("MeshPrimitive: " + util::uint32_t_to_string(weights_0_data_size) +
                                                " bytes of weights_0 data\n");
                                }

                                if (meshPrimitive.HasAttribute("JOINTS_1") && meshPrimitive.HasAttribute("WEIGHTS_1")) {
                                    if (meshPrimitive.TryGetAttributeAccessorId("JOINTS_1", accessorId)) {
                                        const Accessor& accessor = document.accessors.Get(accessorId);

                                        joints_1_data = resourceReader->ReadBinaryData<uint8_t>(document, accessor);
                                        joints_1_data_size = joints_1_data.size() * sizeof(uint8_t);

                                        LOG_NO_ENDL("MeshPrimitive: " + util::uint32_t_to_string(joints_1_data_size) +
                                                    " bytes of joints_1 data\n");
                                    }

                                    if (meshPrimitive.TryGetAttributeAccessorId("WEIGHTS_1", accessorId)) {
                                        const Accessor& accessor = document.accessors.Get(accessorId);

                                        weights_1_data = resourceReader->ReadBinaryData<float>(document, accessor);
                                        weights_1_data_size = weights_1_data.size() * sizeof(float);

                                        LOG_NO_ENDL("MeshPrimitive: " + util::uint32_t_to_string(weights_1_data_size) +
                                                    " bytes of weights_1 data\n");
                                    }

                                    if (static_cast<uint32_t>(joints_0_data.size() / 4) != vertex_count ||
                                        static_cast<uint32_t>(joints_1_data.size() / 4) != vertex_count ||
                                        static_cast<uint32_t>(weights_0_data.size() / 4) != vertex_count ||
                                        static_cast<uint32_t>(weights_1_data.size() / 4) != vertex_count) {
                                        LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                            " weighted data does not conform to IOI's specs (per vertex): JOINTS_0 (4 values), WEIGHTS_0 (4 values), JOINTS_1 (2 values), WEIGHTS_1 (2 values).");

                                        task_status_string =
                                                "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " weighted data does not conform to IOI's specs (per vertex): JOINTS_0 (4 values), WEIGHTS_0 (4 values), JOINTS_1 (2 values), WEIGHTS_1 (2 values).";

                                        task_multiple_status = PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM;

                                        return;
                                    }

                                    for (uint32_t v = 0; v < vertex_count; v++) {
                                        uint32_t x = v * 4;
                                        uint32_t y = v * 4 + 1;
                                        uint32_t z = v * 4 + 2;
                                        uint32_t w = v * 4 + 3;

                                        float weights[] = {weights_0_data[x], weights_0_data[y], weights_0_data[z],
                                                           weights_0_data[w], weights_1_data[x], weights_1_data[y]};
                                        std::vector<std::pair<float, uint32_t>> weights_map;

                                        for (uint32_t w = 0; w < 6; w++) {
                                            if (weights[w] != 0.0f) {
                                                weights_map.push_back(std::pair(weights[w], w));
                                            }
                                        }

                                        std::sort(weights_map.begin(), weights_map.end());

                                        //std::reverse(weights_new.begin(), weights_new.end());

                                        float weights_total = 0.0f;

                                        for (uint32_t w = 0; w < weights_map.size(); w++) {
                                            bool value_found = false;

                                            if (w < (weights_map.size() - 1)) {
                                                for (uint32_t a = 0; a < (prim_float_values.size() - 1); a++) {
                                                    if (weights_map[w].first >= prim_float_values[a] &&
                                                        weights_map[w].first <= prim_float_values[a + 1]) {
                                                        if (!value_found) {
                                                            //if (w == (weights_new.size() - 2))
                                                            //{
                                                            if (prim_float_values[a] == 0.0f) {
                                                                weights_map[w].first = prim_float_values[a + 1];

                                                                value_found = true;
                                                            } else {
                                                                weights_map[w].first = prim_float_values[a];

                                                                value_found = true;
                                                            }
                                                        }
                                                    }
                                                }

                                                weights_total += weights_map[w].first;

                                                weights[weights_map[w].second] = weights_map[w].first;
                                            } else {
                                                for (uint32_t a = 0; a < (prim_float_values.size() - 1); a++) {
                                                    if ((1.0f - weights_total) >= prim_float_values[a] &&
                                                        (1.0f - weights_total) <= prim_float_values[a + 1]) {
                                                        if (!value_found) {
                                                            float diff_a =
                                                                    (1.0f - weights_total) - prim_float_values[a];
                                                            float diff_b =
                                                                    prim_float_values[a + 1] - (1.0f - weights_total);

                                                            if (diff_a < diff_b) {
                                                                weights_map[w].first = prim_float_values[a];

                                                                value_found = true;
                                                            } else {
                                                                weights_map[w].first = prim_float_values[a + 1];

                                                                value_found = true;
                                                            }
                                                        }
                                                    }
                                                }

                                                weights[weights_map[w].second] = weights_map[w].first;
                                            }
                                        }

                                        float total = 0.0f;

                                        for (float weight : weights) {
                                            total += weight;
                                        }

                                        /*for (uint32_t w = 0; w < 6; w++)
                                            {
                                                bool done = false;

                                                for (uint32_t a = 0; a < prim_float_values.size(); a++)
                                                {
                                                    if (weights[w] == prim_float_values[a])
                                                    {
                                                        done = true;
                                                    }
                                                }
                                            }*/

                                        //float weights_total = weights_0_data[x] + weights_0_data[y] + weights_0_data[z] + weights_0_data[w] + weights_1_data[x] + weights_1_data[y];
                                        //float normalize_value = 1.0f / weights_total;

                                        uint8_t temp_uint8_t = static_cast<unsigned char>(std::roundf(
                                                weights[0] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = static_cast<unsigned char>(std::roundf(weights[1] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = static_cast<unsigned char>(std::roundf(weights[2] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = static_cast<unsigned char>(std::roundf(weights[3] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_0_data[x]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_0_data[y]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_0_data[z]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_0_data[w]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = static_cast<unsigned char>(std::roundf(weights[4] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = static_cast<unsigned char>(std::roundf(weights[5] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_1_data[x]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_1_data[y]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);
                                    }
                                } else {
                                    if (static_cast<uint32_t>(joints_0_data.size() / 4) != vertex_count ||
                                        static_cast<uint32_t>(weights_0_data.size() / 4) != vertex_count) {
                                        LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                            " weighted data does not conform to IOI's specs (per vertex): JOINTS_0 (4 values), WEIGHTS_0 (4 values), JOINTS_1 (2 values), WEIGHTS_1 (2 values).");

                                        task_status_string =
                                                "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " weighted data does not conform to IOI's specs (per vertex): JOINTS_0 (4 values), WEIGHTS_0 (4 values), JOINTS_1 (2 values), WEIGHTS_1 (2 values).";

                                        task_multiple_status = PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM;

                                        return;
                                    }

                                    for (uint32_t v = 0; v < vertex_count; v++) {
                                        uint32_t x = v * 4;
                                        uint32_t y = v * 4 + 1;
                                        uint32_t z = v * 4 + 2;
                                        uint32_t w = v * 4 + 3;

                                        float weights[] = {weights_0_data[x], weights_0_data[y], weights_0_data[z],
                                                           weights_0_data[w]};
                                        std::vector<std::pair<float, uint32_t>> weights_map;

                                        for (uint32_t w = 0; w < 4; w++) {
                                            if (weights[w] != 0.0f) {
                                                weights_map.emplace_back(weights[w], w);
                                            }
                                        }

                                        std::sort(weights_map.begin(), weights_map.end());

                                        //std::reverse(weights_new.begin(), weights_new.end());

                                        float weights_total = 0.0f;

                                        for (uint32_t w = 0; w < weights_map.size(); w++) {
                                            bool value_found = false;

                                            if (w < (weights_map.size() - 1)) {
                                                for (uint32_t a = 0; a < (prim_float_values.size() - 1); a++) {
                                                    if (weights_map[w].first >= prim_float_values[a] &&
                                                        weights_map[w].first <= prim_float_values[a + 1]) {
                                                        if (!value_found) {
                                                            //if (w == (weights_new.size() - 2))
                                                            //{
                                                            if (prim_float_values[a] == 0.0f) {
                                                                weights_map[w].first = prim_float_values[a + 1];

                                                                value_found = true;
                                                            } else {
                                                                weights_map[w].first = prim_float_values[a];

                                                                value_found = true;
                                                            }
                                                        }
                                                    }
                                                }

                                                weights_total += weights_map[w].first;

                                                weights[weights_map[w].second] = weights_map[w].first;
                                            } else {
                                                for (uint32_t a = 0; a < (prim_float_values.size() - 1); a++) {
                                                    if ((1.0f - weights_total) >= prim_float_values[a] &&
                                                        (1.0f - weights_total) <= prim_float_values[a + 1]) {
                                                        if (!value_found) {
                                                            float diff_a =
                                                                    (1.0f - weights_total) - prim_float_values[a];
                                                            float diff_b =
                                                                    prim_float_values[a + 1] - (1.0f - weights_total);

                                                            if (diff_a < diff_b) {
                                                                weights_map[w].first = prim_float_values[a];

                                                                value_found = true;
                                                            } else {
                                                                weights_map[w].first = prim_float_values[a + 1];

                                                                value_found = true;
                                                            }
                                                        }
                                                    }
                                                }

                                                weights[weights_map[w].second] = weights_map[w].first;
                                            }
                                        }

                                        float total = 0.0f;

                                        for (float weight : weights) {
                                            total += weight;
                                        }

                                        /*for (uint32_t w = 0; w < 4; w++)
                                            {
                                                bool done = false;

                                                for (uint32_t a = 0; a < prim_float_values.size(); a++)
                                                {
                                                    if (weights[w] == prim_float_values[a])
                                                    {
                                                        done = true;
                                                    }
                                                }
                                            }*/

                                        //float weights_total = weights_0_data[x] + weights_0_data[y] + weights_0_data[z] + weights_0_data[w];
                                        //float normalize_value = 1.0f / weights_total;

                                        auto temp_uint8_t = static_cast<unsigned char>(std::roundf(
                                                weights[0] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = static_cast<unsigned char>(std::roundf(weights[1] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = static_cast<unsigned char>(std::roundf(weights[2] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = static_cast<unsigned char>(std::roundf(weights[3] * 255.0f));

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_0_data[x]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_0_data[y]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_0_data[z]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        temp_uint8_t = glb_bone_indexes[joints_0_data[w]];

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        //temp_uint8_t = static_cast<unsigned char>(std::roundf(weights_1_data[x] * 255.0f));
                                        temp_uint8_t = 0x0;

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        //temp_uint8_t = static_cast<unsigned char>(std::roundf(weights_1_data[y] * 255.0f));
                                        temp_uint8_t = 0x0;

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        //temp_uint8_t = glb_bone_indexes[joints_1_data[x]];
                                        temp_uint8_t = 0x0;

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);

                                        //temp_uint8_t = glb_bone_indexes[joints_1_data[y]];
                                        temp_uint8_t = 0x0;

                                        std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                        prim_file_data.push_back(char1);
                                    }
                                }
                            } else {
                                LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                    " is weighted, but does not have all necessary weighted data: JOINTS_0, WEIGHTS_0, (and if extended weights are used JOINTS_1, WEIGHTS_1).");

                                task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                     " is weighted, but does not have all necessary weighted data: JOINTS_0, WEIGHTS_0, (and if extended weights are used JOINTS_1, WEIGHTS_1).";

                                task_multiple_status = PRIM_REBUILD_WEIGHTED_DATA_MISSING;

                                return;
                            }
                        }

                        if (meshPrimitive.TryGetAttributeAccessorId("NORMAL", accessorId)) {
                            const Accessor& accessor = document.accessors.Get(accessorId);

                            normals_data = resourceReader->ReadBinaryData<float>(document, accessor);
                            normals_data_size = normals_data.size() * sizeof(float);

                            LOG_NO_ENDL("MeshPrimitive: " + util::uint32_t_to_string(normals_data_size) +
                                        " bytes of normal data\n");

                            if (static_cast<uint32_t>(normals_data.size() / 3) != vertex_count) {
                                LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                    " vertex float size (" + util::uint32_t_to_string(vertex_count) +
                                    ") not equal to normal float size (" +
                                    util::uint32_t_to_string((uint32_t) (normals_data.size() / 3)) + ").");

                                task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                     " vertex float size (" + util::uint32_t_to_string(vertex_count) +
                                                     ") not equal to normal float size (" + util::uint32_t_to_string(
                                        static_cast<uint32_t>(normals_data.size() / 3)) + ").";

                                task_multiple_status = PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES;

                                return;
                            }
                        } else {
                            LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                " is missing NORMAL data.");

                            task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                 " is missing NORMAL data.";

                            task_multiple_status = PRIM_REBUILD_MISSING_NORMAL_DATA;

                            return;
                        }

                        if (meshPrimitive.TryGetAttributeAccessorId("TEXCOORD_0", accessorId)) {
                            const Accessor& accessor = document.accessors.Get(accessorId);

                            uvs_data = resourceReader->ReadBinaryData<float>(document, accessor);
                            uvs_data_size = uvs_data.size() * sizeof(float);

                            LOG_NO_ENDL("MeshPrimitive: " + util::uint32_t_to_string(uvs_data_size) +
                                        " bytes of TEXTCOORD_0 data\n");

                            if (static_cast<uint32_t>(uvs_data.size() / 2) != vertex_count) {
                                LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                    " vertex float size (" + util::uint32_t_to_string(vertex_count) +
                                    ") not equal to TEXTCOORD_0 float size (" +
                                    util::uint32_t_to_string((uint32_t) (uvs_data.size() / 2)) + ").");

                                task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                     " vertex float size (" + util::uint32_t_to_string(vertex_count) +
                                                     ") not equal to TEXTCOORD_0 float size (" +
                                                     util::uint32_t_to_string(
                                                             static_cast<uint32_t>(uvs_data.size() / 2)) + ").";

                                task_multiple_status = PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES;

                                return;
                            }
                        } else {
                            LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                " is missing TEXCOORD_0 data.");

                            task_status_string = "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                 " is missing TEXCOORD_0 data.";

                            task_multiple_status = PRIM_REBUILD_MISSING_UV_DATA;

                            return;
                        }

                        std::vector<float> tangents_data(vertices_data.size(), 0);
                        std::vector<float> bitangents_data(vertices_data.size(), 0);

                        MikkTSpace_Data MikkTSpaceData(indices_data, vertices_data, normals_data, tangents_data,
                                                       bitangents_data, uvs_data);

                        SMikkTSpaceInterface MikkTInterface;
                        MikkTInterface.m_getNormal = mikktspace_getNormal;
                        MikkTInterface.m_getNumFaces = mikktspace_getNumFaces;
                        MikkTInterface.m_getNumVerticesOfFace = mikktspace_getNumVerticesOfFace;
                        MikkTInterface.m_getPosition = mikktspace_getPosition;
                        MikkTInterface.m_getTexCoord = mikktspace_getTexCoord;
                        MikkTInterface.m_setTSpaceBasic = mikktspace_setTSpaceBasic;
                        MikkTInterface.m_setTSpace = nullptr;

                        SMikkTSpaceContext MikkTContext;
                        MikkTContext.m_pInterface = &MikkTInterface;
                        MikkTContext.m_pUserData = static_cast<void*>(&MikkTSpaceData);
                        genTangSpaceDefault(&MikkTContext);

                        for (uint32_t v = 0; v < vertex_count; v++) {
                            uint32_t x = v * 2;
                            uint32_t y = v * 2 + 1;

                            if (v == 0) {
                                temp_uvs_min.push_back(uvs_data.at(x));
                                temp_uvs_min.push_back(uvs_data.at(y));
                                temp_uvs_max.push_back(uvs_data.at(x));
                                temp_uvs_max.push_back(uvs_data.at(y));
                            } else {
                                if (uvs_data.at(x) < temp_uvs_min.at(0))
                                    temp_uvs_min.at(0) = uvs_data.at(x);
                                if (uvs_data.at(x) > temp_uvs_max.at(0))
                                    temp_uvs_max.at(0) = uvs_data.at(x);
                                if (uvs_data.at(y) < temp_uvs_min.at(1))
                                    temp_uvs_min.at(1) = uvs_data.at(y);
                                if (uvs_data.at(y) > temp_uvs_max.at(1))
                                    temp_uvs_max.at(1) = uvs_data.at(y);
                            }
                        }

                        for (uint32_t v = 0; v < uvs_compression_scale.size(); v++) {
                            uvs_compression_scale[v] = 0.5 * (temp_uvs_max[v] - temp_uvs_min[v]);
                        }

                        for (uint32_t v = 0; v < uvs_compression_bias.size(); v++) {
                            uvs_compression_bias[v] = temp_uvs_min[v] + uvs_compression_scale[v];
                        }

                        for (uint32_t v = 0; v < vertex_count; v++) {
                            uint32_t x = v * 3;
                            uint32_t y = v * 3 + 1;
                            uint32_t z = v * 3 + 2;

                            auto temp_uint8_t = static_cast<uint8_t>(std::round(
                                    (normals_data[x] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((normals_data[y] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((normals_data[z] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((0.0 + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((tangents_data[x] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((tangents_data[y] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((tangents_data[z] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((0.0 + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((bitangents_data[x] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((bitangents_data[y] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((bitangents_data[z] + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            temp_uint8_t = static_cast<uint8_t>(std::round((0.0 + 1.0) / 2.0 * 255.0));

                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));

                            prim_file_data.push_back(char1);

                            x = v * 2;
                            y = v * 2 + 1;

                            auto temp_short = static_cast<short>(std::roundf(
                                    std::numeric_limits<short>::max() * (uvs_data[x] - uvs_compression_bias[0]) /
                                    uvs_compression_scale[0]));

                            std::memcpy(&char2, &temp_short, sizeof(uint16_t));

                            for (char a : char2) {
                                prim_file_data.push_back(a);
                            }

                            temp_short = static_cast<short>(std::roundf(
                                    std::numeric_limits<short>::max() * (uvs_data[y] - uvs_compression_bias[1]) /
                                    uvs_compression_scale[1]));

                            std::memcpy(&char2, &temp_short, sizeof(uint16_t));

                            for (char a : char2) {
                                prim_file_data.push_back(a);
                            }
                        }

                        if (temp_prim.prim_object_is_weighted.at(m) ||
                            temp_prim.prim_object_headers.at(0).flag_linked) {
                            if (temp_prim.prim_weighted_meshes.at(m).prim_mesh_instance.prim_object_instance.sub_type ==
                                static_cast<uint32_t>(prim::prim_object::SUBTYPE::WEIGHTED) ||
                                ((temp_prim.prim_weighted_sub_meshes.at(m).prim_object_instance.property_flags &
                                  static_cast<uint32_t>(prim::prim_object::PROPERTY_FLAGS::COLOR1)) == 0x0)) {
                                if (meshPrimitive.TryGetAttributeAccessorId("COLOR_0", accessorId)) {
                                    const Accessor& accessor = document.accessors.Get(accessorId);

                                    if (accessor.type == TYPE_VEC4 &&
                                        accessor.componentType == COMPONENT_UNSIGNED_BYTE) {
                                        colors_data_uint8_t = resourceReader->ReadBinaryData<uint8_t>(document,
                                                                                                      accessor);
                                        colors_data_size_uint8_t = colors_data_uint8_t.size() * sizeof(uint8_t);

                                        LOG_NO_ENDL(
                                                "MeshPrimitive: " + util::uint32_t_to_string(colors_data_size_uint8_t) +
                                                " bytes of color data\n");

                                        if (static_cast<uint32_t>(colors_data_uint8_t.size() / 4) != vertex_count) {
                                            LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " color size (" +
                                                util::uint32_t_to_string((uint32_t) (colors_data_uint8_t.size() / 4)) +
                                                ") not equal to vertex size (" +
                                                util::uint32_t_to_string(vertex_count) + ").");

                                            task_status_string =
                                                    "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                    " color size (" + util::uint32_t_to_string(
                                                            static_cast<uint32_t>(colors_data_uint8_t.size() / 4)) +
                                                    ") not equal to vertex size (" +
                                                    util::uint32_t_to_string(vertex_count) + ").";

                                            task_multiple_status = PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES;

                                            return;
                                        }

                                        for (uint32_t v = 0; v < vertex_count; v++) {
                                            uint32_t x = v * 4;
                                            uint32_t y = v * 4 + 1;
                                            uint32_t z = v * 4 + 2;
                                            uint32_t w = v * 4 + 3;

                                            std::memcpy(&char1, &colors_data_uint8_t[x], sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            std::memcpy(&char1, &colors_data_uint8_t[y], sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            std::memcpy(&char1, &colors_data_uint8_t[z], sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            std::memcpy(&char1, &colors_data_uint8_t[w], sizeof(uint8_t));
                                            prim_file_data.push_back(char1);
                                        }
                                    } else if (accessor.type == TYPE_VEC4 &&
                                               accessor.componentType == COMPONENT_UNSIGNED_SHORT) {
                                        colors_data_uint16_t = resourceReader->ReadBinaryData<uint16_t>(document,
                                                                                                        accessor);
                                        colors_data_size_uint16_t = colors_data_uint16_t.size() * sizeof(uint16_t);

                                        LOG_NO_ENDL("MeshPrimitive: " +
                                                    util::uint32_t_to_string(colors_data_size_uint16_t) +
                                                    " bytes of color data\n");

                                        if (static_cast<uint32_t>(colors_data_uint16_t.size() / 4) != vertex_count) {
                                            LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " color size (" +
                                                util::uint32_t_to_string((uint32_t) (colors_data_uint16_t.size() / 4)) +
                                                ") not equal to vertex size (" +
                                                util::uint32_t_to_string(vertex_count) + ").");

                                            task_status_string =
                                                    "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                    " color size (" + util::uint32_t_to_string(
                                                            static_cast<uint32_t>(colors_data_uint16_t.size() / 4)) +
                                                    ") not equal to vertex size (" +
                                                    util::uint32_t_to_string(vertex_count) + ").";

                                            task_multiple_status = PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES;

                                            return;
                                        }

                                        for (uint32_t v = 0; v < vertex_count; v++) {
                                            uint32_t x = v * 4;
                                            uint32_t y = v * 4 + 1;
                                            uint32_t z = v * 4 + 2;
                                            uint32_t w = v * 4 + 3;

                                            uint8_t temp_uint8_t = std::round(
                                                    (static_cast<double>(colors_data_uint16_t[x]) /
                                                     static_cast<double>(0xFFFF)) * static_cast<double>(0xFF));

                                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            temp_uint8_t = std::round(
                                                    (double) ((colors_data_uint16_t[y]) / static_cast<double>(0xFFFF)) *
                                                    static_cast<double>(0xFF));

                                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            temp_uint8_t = std::round(
                                                    (double) ((colors_data_uint16_t[z]) / static_cast<double>(0xFFFF)) *
                                                    static_cast<double>(0xFF));

                                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            temp_uint8_t = std::round(
                                                    (double) ((colors_data_uint16_t[w]) / static_cast<double>(0xFFFF)) *
                                                    static_cast<double>(0xFF));

                                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                            prim_file_data.push_back(char1);
                                        }
                                    } else {
                                        LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                            " color data is of the wrong format, needs to be of type VEC4, an unsigned byte or short, and normalized.");

                                        task_status_string =
                                                "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " color data is of the wrong format, needs to be of type VEC4, an unsigned byte or short, and normalized.";

                                        task_multiple_status = PRIM_REBUILD_COLORS_WRONG_FORMAT;

                                        return;
                                    }
                                } else {
                                    LOG("Warning: GLB does not have color data (COLOR_0), but PRIM requires it, filling colors with 0xFF.");

                                    for (uint32_t a = 0; a < (vertex_count * 0x4); a++) {
                                        prim_file_data.push_back(0xFF);
                                    }
                                }
                            }
                        } else {
                            if (temp_prim.prim_meshes.at(m).prim_object_instance.sub_type ==
                                static_cast<uint32_t>(prim::prim_object::SUBTYPE::WEIGHTED) ||
                                ((temp_prim.prim_sub_meshes.at(m).prim_object_instance.property_flags &
                                  static_cast<uint32_t>(prim::prim_object::PROPERTY_FLAGS::COLOR1)) == 0x0)) {
                                if (meshPrimitive.TryGetAttributeAccessorId("COLOR_0", accessorId)) {
                                    const Accessor& accessor = document.accessors.Get(accessorId);

                                    if (accessor.type == TYPE_VEC4 &&
                                        accessor.componentType == COMPONENT_UNSIGNED_BYTE) {
                                        colors_data_uint8_t = resourceReader->ReadBinaryData<uint8_t>(document,
                                                                                                      accessor);
                                        colors_data_size_uint8_t = colors_data_uint8_t.size() * sizeof(uint8_t);

                                        LOG_NO_ENDL(
                                                "MeshPrimitive: " + util::uint32_t_to_string(colors_data_size_uint8_t) +
                                                " bytes of color data\n");

                                        if (static_cast<uint32_t>(colors_data_uint8_t.size() / 4) != vertex_count) {
                                            LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " color size (" +
                                                util::uint32_t_to_string((uint32_t) (colors_data_uint8_t.size() / 4)) +
                                                ") not equal to vertex size (" +
                                                util::uint32_t_to_string(vertex_count) + ").");

                                            task_status_string =
                                                    "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                    " color size (" + util::uint32_t_to_string(
                                                            static_cast<uint32_t>(colors_data_uint8_t.size() / 4)) +
                                                    ") not equal to vertex size (" +
                                                    util::uint32_t_to_string(vertex_count) + ").";

                                            task_multiple_status = PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES;

                                            return;
                                        }

                                        for (uint32_t v = 0; v < vertex_count; v++) {
                                            uint32_t x = v * 4;
                                            uint32_t y = v * 4 + 1;
                                            uint32_t z = v * 4 + 2;
                                            uint32_t w = v * 4 + 3;

                                            std::memcpy(&char1, &colors_data_uint8_t[x], sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            std::memcpy(&char1, &colors_data_uint8_t[y], sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            std::memcpy(&char1, &colors_data_uint8_t[z], sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            std::memcpy(&char1, &colors_data_uint8_t[w], sizeof(uint8_t));
                                            prim_file_data.push_back(char1);
                                        }
                                    } else if (accessor.type == TYPE_VEC4 &&
                                               accessor.componentType == COMPONENT_UNSIGNED_SHORT) {
                                        colors_data_uint16_t = resourceReader->ReadBinaryData<uint16_t>(document,
                                                                                                        accessor);
                                        colors_data_size_uint16_t = colors_data_uint16_t.size() * sizeof(uint16_t);

                                        LOG_NO_ENDL("MeshPrimitive: " +
                                                    util::uint32_t_to_string(colors_data_size_uint16_t) +
                                                    " bytes of color data\n");

                                        if (static_cast<uint32_t>(colors_data_uint16_t.size() / 4) != vertex_count) {
                                            LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " color size (" +
                                                util::uint32_t_to_string((uint32_t) (colors_data_uint16_t.size() / 4)) +
                                                ") not equal to vertex size (" +
                                                util::uint32_t_to_string(vertex_count) + ").");

                                            task_status_string =
                                                    "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                    " color size (" + util::uint32_t_to_string(
                                                            static_cast<uint32_t>(colors_data_uint16_t.size() / 4)) +
                                                    ") not equal to vertex size (" +
                                                    util::uint32_t_to_string(vertex_count) + ").";

                                            task_multiple_status = PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES;

                                            return;
                                        }

                                        for (uint32_t v = 0; v < vertex_count; v++) {
                                            uint32_t x = v * 4;
                                            uint32_t y = v * 4 + 1;
                                            uint32_t z = v * 4 + 2;
                                            uint32_t w = v * 4 + 3;

                                            uint8_t temp_uint8_t = std::round(
                                                    (static_cast<double>(colors_data_uint16_t[x]) /
                                                     static_cast<double>(0xFFFF)) * static_cast<double>(0xFF));

                                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            temp_uint8_t = std::round(
                                                    ((colors_data_uint16_t[y]) / static_cast<double>(0xFFFF)) *
                                                    static_cast<double>(0xFF));

                                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            temp_uint8_t = std::round(
                                                    ((colors_data_uint16_t[z]) / static_cast<double>(0xFFFF)) *
                                                    static_cast<double>(0xFF));

                                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                            prim_file_data.push_back(char1);

                                            temp_uint8_t = std::round(
                                                    ((colors_data_uint16_t[w]) / static_cast<double>(0xFFFF)) *
                                                    static_cast<double>(0xFF));

                                            std::memcpy(&char1, &temp_uint8_t, sizeof(uint8_t));
                                            prim_file_data.push_back(char1);
                                        }
                                    } else {
                                        LOG("Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                            " color data is of the wrong format, needs to be of type VEC4, an unsigned byte or short, and normalized.");

                                        task_status_string =
                                                "Error: " + glb_file_names.at(j) + "'s mesh " + temp_mesh.name +
                                                " color data is of the wrong format, needs to be of type VEC4, an unsigned byte or short, and normalized.";

                                        task_multiple_status = PRIM_REBUILD_COLORS_WRONG_FORMAT;

                                        return;
                                    }
                                } else {
                                    LOG("Warning: GLB does not have color data (COLOR_0), but PRIM requires it, filling colors with 0xFF.");

                                    for (uint32_t a = 0; a < (vertex_count * 0x4); a++) {
                                        prim_file_data.push_back(0xFF);
                                    }
                                }
                            }
                        }

                        while (prim_file_data.size() % 0x10 != 0) {
                            prim_file_data.push_back(0x0);
                        }

                        if (temp_prim.prim_object_is_weighted.at(m) ||
                            temp_prim.prim_object_headers.at(0).flag_linked) {
                            if (temp_prim.prim_weighted_sub_meshes.at(m).collision_offset) {
                                offset_collision = prim_file_data.size();

                                for (char a : temp_prim.prim_weighted_sub_meshes.at(m).collision_data) {
                                    prim_file_data.push_back(a);
                                }

                                while (prim_file_data.size() % 0x10 != 0) {
                                    prim_file_data.push_back(0x0);
                                }
                            }

                            if (temp_prim.prim_weighted_sub_meshes.at(m).cloth_offset) {
                                offset_cloth = prim_file_data.size();

                                for (char a : temp_prim.prim_weighted_sub_meshes.at(m).cloth_data) {
                                    prim_file_data.push_back(a);
                                }

                                while (prim_file_data.size() % 0x10 != 0) {
                                    prim_file_data.push_back(0x0);
                                }
                            }
                        } else {
                            if (temp_prim.prim_sub_meshes.at(m).collision_offset) {
                                offset_collision = prim_file_data.size();

                                for (char a : temp_prim.prim_sub_meshes.at(m).collision_data) {
                                    prim_file_data.push_back(a);
                                }

                                while (prim_file_data.size() % 0x10 != 0) {
                                    prim_file_data.push_back(0x0);
                                }
                            }

                            if (temp_prim.prim_sub_meshes.at(m).cloth_offset) {
                                offset_cloth = prim_file_data.size();

                                for (char a : temp_prim.prim_sub_meshes.at(m).cloth_data) {
                                    prim_file_data.push_back(a);
                                }

                                while (prim_file_data.size() % 0x10 != 0) {
                                    prim_file_data.push_back(0x0);
                                }
                            }
                        }

                        offset_sub_mesh = prim_file_data.size();

                        {
                            uint8_t temp_draw = 0;
                            uint8_t temp_pack = 0;
                            uint16_t temp_type = 0;

                            uint8_t temp_sub_type = 0;
                            uint8_t temp_property_flags = 0;
                            uint8_t temp_lod = 0;
                            uint8_t temp_variant = 0;
                            uint8_t temp_bias = 0;
                            uint8_t temp_offset = 0;
                            uint16_t temp_material_id = 0;
                            uint32_t temp_wire_color = 0;
                            uint32_t temp_color = 0;
                            vector3 temp_bounding_box_min;
                            vector3 temp_bounding_box_max;

                            uint32_t temp_vertex_count = 0;
                            uint32_t temp_vertex_buffer_offset = 0;
                            uint32_t temp_index_count = 0;
                            uint32_t temp_index_count_additional = 0;
                            uint32_t temp_index_buffer_offset = 0;
                            uint32_t temp_collision_offset = 0;
                            uint32_t temp_cloth_offset = 0;
                            uint32_t temp_uv_channel_count = 0;
                            uint32_t temp_unknown_offset = 0;

                            if (temp_prim.prim_object_is_weighted.at(m) ||
                                temp_prim.prim_object_headers.at(0).flag_linked) {
                                temp_draw = temp_prim.prim_weighted_sub_meshes.at(
                                        m).prim_object_instance.prim_header_instance.draw;
                                temp_pack = temp_prim.prim_weighted_sub_meshes.at(
                                        m).prim_object_instance.prim_header_instance.pack;
                                temp_type = temp_prim.prim_weighted_sub_meshes.at(
                                        m).prim_object_instance.prim_header_instance.type;

                                temp_sub_type = temp_prim.prim_weighted_sub_meshes.at(m).prim_object_instance.sub_type;
                                temp_property_flags = temp_prim.prim_weighted_sub_meshes.at(
                                        m).prim_object_instance.property_flags;
                                temp_lod = temp_prim.prim_weighted_sub_meshes.at(m).prim_object_instance.lod;
                                temp_variant = temp_prim.prim_weighted_sub_meshes.at(m).prim_object_instance.variant;
                                temp_bias = temp_prim.prim_weighted_sub_meshes.at(m).prim_object_instance.bias;
                                temp_offset = temp_prim.prim_weighted_sub_meshes.at(m).prim_object_instance.offset;
                                temp_material_id = temp_prim.prim_weighted_sub_meshes.at(
                                        m).prim_object_instance.material_id;
                                temp_wire_color = temp_prim.prim_weighted_sub_meshes.at(
                                        m).prim_object_instance.wire_color;
                                temp_color = temp_prim.prim_weighted_sub_meshes.at(m).prim_object_instance.color;
                                temp_bounding_box_min.x = temp_vertexes_min[0];
                                temp_bounding_box_min.y = temp_vertexes_min[1];
                                temp_bounding_box_min.z = temp_vertexes_min[2];
                                temp_bounding_box_max.x = temp_vertexes_max[0];
                                temp_bounding_box_max.y = temp_vertexes_max[1];
                                temp_bounding_box_max.z = temp_vertexes_max[2];

                                temp_vertex_count = vertex_count;
                                temp_vertex_buffer_offset = offset_vertices;
                                temp_index_count = indices_count;
                                temp_index_count_additional = temp_prim.prim_weighted_sub_meshes.at(
                                        m).index_count_additional;
                                temp_index_buffer_offset = offset_indices;
                                temp_collision_offset = offset_collision;
                                temp_cloth_offset = offset_cloth;
                                temp_uv_channel_count = temp_prim.prim_weighted_sub_meshes.at(m).uv_channel_count;
                                temp_unknown_offset = temp_prim.prim_weighted_sub_meshes.at(m).unknown_offset;
                            } else {
                                temp_draw = temp_prim.prim_sub_meshes.at(
                                        m).prim_object_instance.prim_header_instance.draw;
                                temp_pack = temp_prim.prim_sub_meshes.at(
                                        m).prim_object_instance.prim_header_instance.pack;
                                temp_type = temp_prim.prim_sub_meshes.at(
                                        m).prim_object_instance.prim_header_instance.type;

                                temp_sub_type = temp_prim.prim_sub_meshes.at(m).prim_object_instance.sub_type;
                                temp_property_flags = temp_prim.prim_sub_meshes.at(
                                        m).prim_object_instance.property_flags;
                                temp_lod = temp_prim.prim_sub_meshes.at(m).prim_object_instance.lod;
                                temp_variant = temp_prim.prim_sub_meshes.at(m).prim_object_instance.variant;
                                temp_bias = temp_prim.prim_sub_meshes.at(m).prim_object_instance.bias;
                                temp_offset = temp_prim.prim_sub_meshes.at(m).prim_object_instance.offset;
                                temp_material_id = temp_prim.prim_sub_meshes.at(m).prim_object_instance.material_id;
                                temp_wire_color = temp_prim.prim_sub_meshes.at(m).prim_object_instance.wire_color;
                                temp_color = temp_prim.prim_sub_meshes.at(m).prim_object_instance.color;
                                temp_bounding_box_min.x = temp_vertexes_min[0];
                                temp_bounding_box_min.y = temp_vertexes_min[1];
                                temp_bounding_box_min.z = temp_vertexes_min[2];
                                temp_bounding_box_max.x = temp_vertexes_max[0];
                                temp_bounding_box_max.y = temp_vertexes_max[1];
                                temp_bounding_box_max.z = temp_vertexes_max[2];

                                temp_vertex_count = vertex_count;
                                temp_vertex_buffer_offset = offset_vertices;
                                temp_index_count = indices_count;
                                temp_index_count_additional = temp_prim.prim_sub_meshes.at(m).index_count_additional;
                                temp_index_buffer_offset = offset_indices;
                                temp_collision_offset = offset_collision;
                                temp_cloth_offset = offset_cloth;
                                temp_uv_channel_count = temp_prim.prim_sub_meshes.at(m).uv_channel_count;
                                temp_unknown_offset = temp_prim.prim_sub_meshes.at(m).unknown_offset;
                            }

                            std::memcpy(&char1, &temp_draw, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_pack, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char2, &temp_type, sizeof(uint16_t));
                            for (char a : char2) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char1, &temp_sub_type, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_property_flags, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_lod, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_variant, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_bias, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_offset, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char2, &temp_material_id, sizeof(uint16_t));
                            for (char a : char2) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_wire_color, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_color, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_min.x, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_min.y, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_min.z, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_max.x, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_max.y, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_max.z, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_vertex_count, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_vertex_buffer_offset, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_index_count, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_index_count_additional, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_index_buffer_offset, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_collision_offset, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_cloth_offset, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_uv_channel_count, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_unknown_offset, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            while (prim_file_data.size() % 0x10 != 0) {
                                prim_file_data.push_back(0x0);
                            }
                        }

                        offset_sub_mesh_pointer = prim_file_data.size();

                        std::memcpy(&char4, &offset_sub_mesh, sizeof(uint32_t));
                        for (char a : char4) {
                            prim_file_data.push_back(a);
                        }

                        while (prim_file_data.size() % 0x10 != 0) {
                            prim_file_data.push_back(0x0);
                        }

                        if (temp_prim.prim_object_is_weighted.at(m)) {
                            if (temp_prim.prim_weighted_meshes.at(m).bones_nodes_offset) {
                                offset_bones_nodes = prim_file_data.size();

                                for (unsigned int& a : temp_prim.prim_weighted_sub_meshes.at(m).bones_nodes_data) {
                                    std::memcpy(&char4, &a, sizeof(uint32_t));

                                    for (char b : char4) {
                                        prim_file_data.push_back(b);
                                    }
                                }

                                while (prim_file_data.size() % 0x10 != 0) {
                                    prim_file_data.push_back(0x0);
                                }
                            }

                            if (temp_prim.prim_weighted_meshes.at(m).bone_info_offset) {
                                offset_bones_info = prim_file_data.size();

                                for (unsigned char a : temp_prim.prim_weighted_sub_meshes.at(m).bones_info_data) {
                                    prim_file_data.push_back(a);
                                }

                                while (prim_file_data.size() % 0x10 != 0) {
                                    prim_file_data.push_back(0x0);
                                }
                            }

                            if (temp_prim.prim_weighted_meshes.at(m).bone_indices_offset) {
                                offset_bones_indices = prim_file_data.size();

                                uint32_t temp_bone_indicies_data_size = temp_prim.prim_weighted_sub_meshes.at(
                                        m).bones_indices_data.size();
                                temp_bone_indicies_data_size += 0x2;

                                std::memcpy(&char4, &temp_bone_indicies_data_size, sizeof(uint32_t));

                                for (char b : char4) {
                                    prim_file_data.push_back(b);
                                }

                                for (unsigned short& a : temp_prim.prim_weighted_sub_meshes.at(m).bones_indices_data) {
                                    std::memcpy(&char2, &a, sizeof(uint16_t));

                                    for (char b : char2) {
                                        prim_file_data.push_back(b);
                                    }
                                }

                                while (prim_file_data.size() % 0x10 != 0) {
                                    prim_file_data.push_back(0x0);
                                }
                            }
                        }

                        prim_file_mesh_object_offsets.push_back(prim_file_data.size());

                        {
                            uint8_t temp_draw = 0;
                            uint8_t temp_pack = 0;
                            uint16_t temp_type = 0;

                            uint8_t temp_sub_type = 0;
                            uint8_t temp_property_flags = 0;
                            uint8_t temp_lod = 0;
                            uint8_t temp_variant = 0;
                            uint8_t temp_bias = 0;
                            uint8_t temp_offset = 0;
                            uint16_t temp_material_id = 0;
                            uint32_t temp_wire_color = 0;
                            uint32_t temp_color = 0;
                            vector3 temp_bounding_box_min;
                            vector3 temp_bounding_box_max;

                            uint32_t sub_mesh_table_offset_pointer = 0;
                            vector4 vertex_position_scale;
                            vector4 vertex_position_bias;
                            vector2 uv_position_scale;
                            vector2 uv_position_bias;
                            uint32_t cloth_flags = 0;

                            uint32_t bones_nodes_count = 0;
                            uint32_t bones_nodes_offset = 0;
                            uint32_t bone_indices_offset = 0;
                            uint32_t bone_info_offset = 0;

                            if (temp_prim.prim_object_is_weighted.at(m) ||
                                temp_prim.prim_object_headers.at(0).flag_linked) {
                                temp_draw = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.prim_header_instance.draw;
                                temp_pack = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.prim_header_instance.pack;
                                temp_type = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.prim_header_instance.type;

                                temp_sub_type = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.sub_type;
                                temp_property_flags = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.property_flags;

                                if (lods_match) {
                                    temp_lod = temp_prim.prim_weighted_meshes.at(
                                            m).prim_mesh_instance.prim_object_instance.lod;
                                } else {
                                    temp_lod = 0xFF;
                                }

                                temp_variant = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.variant;
                                temp_bias = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.bias;
                                temp_offset = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.offset;
                                temp_material_id = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.material_id;
                                temp_wire_color = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.wire_color;
                                temp_color = temp_prim.prim_weighted_meshes.at(
                                        m).prim_mesh_instance.prim_object_instance.color;
                                temp_bounding_box_min.x = temp_vertexes_min[0];
                                temp_bounding_box_min.y = temp_vertexes_min[1];
                                temp_bounding_box_min.z = temp_vertexes_min[2];
                                temp_bounding_box_max.x = temp_vertexes_max[0];
                                temp_bounding_box_max.y = temp_vertexes_max[1];
                                temp_bounding_box_max.z = temp_vertexes_max[2];

                                sub_mesh_table_offset_pointer = offset_sub_mesh_pointer;
                                vertex_position_scale.x = vertices_compression_scale[0];
                                vertex_position_scale.y = vertices_compression_scale[1];
                                vertex_position_scale.z = vertices_compression_scale[2];
                                vertex_position_scale.w = 0.5;
                                vertex_position_bias.x = vertices_compression_bias[0];
                                vertex_position_bias.y = vertices_compression_bias[1];
                                vertex_position_bias.z = vertices_compression_bias[2];
                                vertex_position_bias.w = 0.5;
                                uv_position_scale.x = uvs_compression_scale[0];
                                uv_position_scale.y = uvs_compression_scale[1];
                                uv_position_bias.x = uvs_compression_bias[0];
                                uv_position_bias.y = uvs_compression_bias[1];
                                cloth_flags = temp_prim.prim_weighted_meshes.at(m).prim_mesh_instance.cloth_flags;

                                bones_nodes_count = temp_prim.prim_weighted_meshes.at(m).bones_nodes_count;
                                bones_nodes_offset = offset_bones_nodes;
                                bone_indices_offset = offset_bones_indices;
                                bone_info_offset = offset_bones_info;
                            } else {
                                temp_draw = temp_prim.prim_meshes.at(m).prim_object_instance.prim_header_instance.draw;
                                temp_pack = temp_prim.prim_meshes.at(m).prim_object_instance.prim_header_instance.pack;
                                temp_type = temp_prim.prim_meshes.at(m).prim_object_instance.prim_header_instance.type;

                                temp_sub_type = temp_prim.prim_meshes.at(m).prim_object_instance.sub_type;
                                temp_property_flags = temp_prim.prim_meshes.at(m).prim_object_instance.property_flags;

                                if (lods_match) {
                                    temp_lod = temp_prim.prim_meshes.at(m).prim_object_instance.lod;
                                } else {
                                    temp_lod = 0xFF;
                                }

                                temp_variant = temp_prim.prim_meshes.at(m).prim_object_instance.variant;
                                temp_bias = temp_prim.prim_meshes.at(m).prim_object_instance.bias;
                                temp_offset = temp_prim.prim_meshes.at(m).prim_object_instance.offset;
                                temp_material_id = temp_prim.prim_meshes.at(m).prim_object_instance.material_id;
                                temp_wire_color = temp_prim.prim_meshes.at(m).prim_object_instance.wire_color;
                                temp_color = temp_prim.prim_meshes.at(m).prim_object_instance.color;
                                temp_bounding_box_min.x = temp_vertexes_min[0];
                                temp_bounding_box_min.y = temp_vertexes_min[1];
                                temp_bounding_box_min.z = temp_vertexes_min[2];
                                temp_bounding_box_max.x = temp_vertexes_max[0];
                                temp_bounding_box_max.y = temp_vertexes_max[1];
                                temp_bounding_box_max.z = temp_vertexes_max[2];

                                sub_mesh_table_offset_pointer = offset_sub_mesh_pointer;
                                vertex_position_scale.x = vertices_compression_scale[0];
                                vertex_position_scale.y = vertices_compression_scale[1];
                                vertex_position_scale.z = vertices_compression_scale[2];
                                vertex_position_scale.w = 0.5;
                                vertex_position_bias.x = vertices_compression_bias[0];
                                vertex_position_bias.y = vertices_compression_bias[1];
                                vertex_position_bias.z = vertices_compression_bias[2];
                                vertex_position_bias.w = 0.5;
                                uv_position_scale.x = uvs_compression_scale[0];
                                uv_position_scale.y = uvs_compression_scale[1];
                                uv_position_bias.x = uvs_compression_bias[0];
                                uv_position_bias.y = uvs_compression_bias[1];
                                cloth_flags = temp_prim.prim_meshes.at(m).cloth_flags;
                            }

                            std::memcpy(&char1, &temp_draw, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_pack, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char2, &temp_type, sizeof(uint16_t));
                            for (char a : char2) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char1, &temp_sub_type, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_property_flags, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_lod, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_variant, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_bias, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char1, &temp_offset, sizeof(uint8_t));
                            prim_file_data.push_back(char1);

                            std::memcpy(&char2, &temp_material_id, sizeof(uint16_t));
                            for (char a : char2) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_wire_color, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_color, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_min.x, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_min.y, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_min.z, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_max.x, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_max.y, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &temp_bounding_box_max.z, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &sub_mesh_table_offset_pointer, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &vertex_position_scale.x, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &vertex_position_scale.y, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &vertex_position_scale.z, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &vertex_position_scale.w, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &vertex_position_bias.x, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &vertex_position_bias.y, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &vertex_position_bias.z, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &vertex_position_bias.w, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &uv_position_scale.x, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &uv_position_scale.y, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &uv_position_bias.x, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &uv_position_bias.y, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            std::memcpy(&char4, &cloth_flags, sizeof(uint32_t));
                            for (char a : char4) {
                                prim_file_data.push_back(a);
                            }

                            if (temp_prim.prim_object_is_weighted.at(m)) {
                                std::memcpy(&char4, &bones_nodes_count, sizeof(uint32_t));
                                for (char a : char4) {
                                    prim_file_data.push_back(a);
                                }

                                std::memcpy(&char4, &bones_nodes_offset, sizeof(uint32_t));
                                for (char a : char4) {
                                    prim_file_data.push_back(a);
                                }

                                std::memcpy(&char4, &bone_indices_offset, sizeof(uint32_t));
                                for (char a : char4) {
                                    prim_file_data.push_back(a);
                                }

                                std::memcpy(&char4, &bone_info_offset, sizeof(uint32_t));
                                for (char a : char4) {
                                    prim_file_data.push_back(a);
                                }
                            }

                            while (prim_file_data.size() % 0x10 != 0) {
                                prim_file_data.push_back(0x0);
                            }
                        }
                    }
                }
            }

            uint32_t offset_mesh_object_offsets_pointer = prim_file_data.size();

            for (unsigned int& prim_file_mesh_object_offset : prim_file_mesh_object_offsets) {
                char char4[4];

                std::memcpy(&char4, &prim_file_mesh_object_offset, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }
            }

            while (prim_file_data.size() % 0x10 != 0) {
                prim_file_data.push_back(0x0);
            }

            if (temp_prim.prim_object_headers.size() != 1) {
                LOG("Error: " + glb_file_names.at(j) + "'s has too many primary object headers.");

                task_status_string = "Error: " + glb_file_names.at(j) + "'s has too many primary object headers.";

                task_multiple_status = PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS;

                return;
            }

            char char1;
            char char2[2];
            char char4[4];

            uint32_t primary_header_table_offset = prim_file_data.size();

            std::memcpy(&char4, &primary_header_table_offset, sizeof(uint32_t));
            for (uint32_t a = 0; a < sizeof(uint32_t); a++) {
                prim_file_data[a] = char4[a];
            }

            {
                uint8_t temp_draw = temp_prim.prim_object_headers.at(0).prim_header_instance.draw;
                uint8_t temp_pack = temp_prim.prim_object_headers.at(0).prim_header_instance.pack;
                uint16_t temp_type = temp_prim.prim_object_headers.at(0).prim_header_instance.type;

                uint32_t temp_property_flags = temp_prim.prim_object_headers.at(0).property_flags;
                uint32_t temp_borg_index = temp_prim.prim_object_headers.at(0).borg_index;

                uint32_t temp_object_count = 0;

                if (lods_match) {
                    temp_object_count = temp_prim.prim_object_headers.at(0).object_count;
                } else {
                    temp_object_count = object_count_total;
                }

                uint32_t temp_object_offset = offset_mesh_object_offsets_pointer;
                vector3 temp_bounding_box_min;
                vector3 temp_bounding_box_max;
                temp_bounding_box_min.x = vertices_min_all[0];
                temp_bounding_box_min.y = vertices_min_all[1];
                temp_bounding_box_min.z = vertices_min_all[2];
                temp_bounding_box_max.x = vertices_max_all[0];
                temp_bounding_box_max.y = vertices_max_all[1];
                temp_bounding_box_max.z = vertices_max_all[2];

                std::memcpy(&char1, &temp_draw, sizeof(uint8_t));
                prim_file_data.push_back(char1);

                std::memcpy(&char1, &temp_pack, sizeof(uint8_t));
                prim_file_data.push_back(char1);

                std::memcpy(&char2, &temp_type, sizeof(uint16_t));
                for (char a : char2) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_property_flags, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_borg_index, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_object_count, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_object_offset, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_bounding_box_min.x, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_bounding_box_min.y, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_bounding_box_min.z, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_bounding_box_max.x, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_bounding_box_max.y, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                std::memcpy(&char4, &temp_bounding_box_max.z, sizeof(uint32_t));
                for (char a : char4) {
                    prim_file_data.push_back(a);
                }

                while (prim_file_data.size() % 0x10 != 0) {
                    prim_file_data.push_back(0x0);
                }
            }

            std::string current_path = prim_folders.at(i) + "\\REBUILT";

            file::create_directories(current_path);

            std::ofstream prim_file = std::ofstream(current_path + "\\" + prim_file_names.at(j), std::ofstream::binary);

            if (!prim_file.good()) {
                LOG_AND_EXIT(
                        "Error: PRIM file " + current_path + "\\" + prim_file_names.at(j) + " could not be created.");
            }

            prim_file.write(prim_file_header_data.data(), prim_file_header_data.size());

            prim_file.write(prim_file_data.data(), prim_file_data.size());

            prim_file.close();

            std::filesystem::copy(prim_folders.at(i) + "\\metas\\" + prim_file_names.at(j) + ".meta",
                                  current_path + "\\" + prim_file_names.at(j) + ".meta",
                                  std::filesystem::copy_options::overwrite_existing);

            current_path = rpkgs_path + "\\" + temp_prim.rpkg_output_file;

            if (generate_rpkgs) {
                file::create_directories(current_path);
            }

            directory_set.insert(current_path);

            if (generate_rpkgs) {
                std::filesystem::copy(prim_folders.at(i) + "\\REBUILT\\" + prim_file_names.at(j),
                                      current_path + "\\" + prim_file_names.at(j),
                                      std::filesystem::copy_options::overwrite_existing);

                std::filesystem::copy(prim_folders.at(i) + "\\REBUILT\\" + prim_file_names.at(j) + ".meta",
                                      current_path + "\\" + prim_file_names.at(j) + ".meta",
                                      std::filesystem::copy_options::overwrite_existing);
            }

            std::ofstream rpkg_file_name_file = std::ofstream(prim_folders.at(i) + "\\REBUILT\\rpkgfilename.txt",
                                                              std::ofstream::binary);

            if (!rpkg_file_name_file.good()) {
                LOG_AND_EXIT("Error: RPKG file name file " + prim_folders.at(i) + "\\REBUILT\\rpkgfilename.txt" +
                             " could not be created.");
            }

            rpkg_file_name_file.write(temp_prim.rpkg_output_file.c_str(), temp_prim.rpkg_output_file.length());

            rpkg_file_name_file.close();
        }
    }

    if (generate_rpkgs) {
        for (auto it = directory_set.begin(); it != directory_set.end(); it++) {
            std::string input_path = *it;
            std::string filter = "";
            std::string output_path_string = rpkgs_path;

            //std::cout << *it << std::endl;
            //std::cout << output_path_string << std::endl;

            rpkg_function::generate_rpkg_from(input_path, output_path_string, true);
        }
    }

    LOG("Done");

    percent_progress = static_cast<uint32_t>(100);

    task_single_status = TASK_SUCCESSFUL;

    task_multiple_status = PRIM_REBUILD_SUCCESSFUL;
}
