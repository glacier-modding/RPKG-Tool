#include "gltf.h"
#include <GLTFSDK/BufferBuilder.h>
#include <GLTFSDK/IStreamWriter.h>
#include <GLTFSDK/GLBResourceWriter.h>
#include <GLTFSDK/Serialize.h>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cassert>

using namespace Microsoft::glTF;

class StreamWriter : public IStreamWriter
{
public:
    explicit StreamWriter(std::filesystem::path pathBase) : m_pathBase(std::move(pathBase))
    {
        assert(m_pathBase.has_root_path());
    }

    [[nodiscard]] std::shared_ptr<std::ostream> GetOutputStream(const std::string& filename) const override
    {
        auto streamPath = m_pathBase / std::filesystem::u8path(filename);
        auto stream = std::make_shared<std::ofstream>(streamPath, std::ios_base::binary);

        if (!stream || !(*stream))
        {
            throw std::runtime_error("Unable to create a valid output stream for uri: " + filename);
        }

        return stream;
    }

private:
    std::filesystem::path m_pathBase;
};

void gltf::output_to_single_file(asset3ds& asset3ds_data, std::string& file_path, int type, bool rotate)
{
    std::vector<std::vector<float>> vertexes_min;
    std::vector<std::vector<float>> vertexes_max;

    for (uint32_t a = 0; a < asset3ds_data.indexes.size(); a++)
    {
        std::vector<float> temp_vertexes_min;
        std::vector<float> temp_vertexes_max;

        for (uint32_t v = 0; v < (asset3ds_data.vertexes.at(a).size() / 3); v++)
        {
            uint32_t x = v * 3;
            uint32_t y = v * 3 + 1;
            uint32_t z = v * 3 + 2;

            if (v == 0)
            {
                temp_vertexes_min.push_back(asset3ds_data.vertexes.at(a).at(x));
                temp_vertexes_min.push_back(asset3ds_data.vertexes.at(a).at(y));
                temp_vertexes_min.push_back(asset3ds_data.vertexes.at(a).at(z));
                temp_vertexes_max.push_back(asset3ds_data.vertexes.at(a).at(x));
                temp_vertexes_max.push_back(asset3ds_data.vertexes.at(a).at(y));
                temp_vertexes_max.push_back(asset3ds_data.vertexes.at(a).at(z));
            }
            else
            {
                if (asset3ds_data.vertexes.at(a).at(x) < temp_vertexes_min.at(0))
                    temp_vertexes_min.at(0) = asset3ds_data.vertexes.at(a).at(x);
                if (asset3ds_data.vertexes.at(a).at(x) > temp_vertexes_max.at(0))
                    temp_vertexes_max.at(0) = asset3ds_data.vertexes.at(a).at(x);
                if (asset3ds_data.vertexes.at(a).at(y) < temp_vertexes_min.at(1))
                    temp_vertexes_min.at(1) = asset3ds_data.vertexes.at(a).at(y);
                if (asset3ds_data.vertexes.at(a).at(y) > temp_vertexes_max.at(1))
                    temp_vertexes_max.at(1) = asset3ds_data.vertexes.at(a).at(y);
                if (asset3ds_data.vertexes.at(a).at(z) < temp_vertexes_min.at(2))
                    temp_vertexes_min.at(2) = asset3ds_data.vertexes.at(a).at(z);
                if (asset3ds_data.vertexes.at(a).at(z) > temp_vertexes_max.at(2))
                    temp_vertexes_max.at(2) = asset3ds_data.vertexes.at(a).at(z);
            }
        }

        vertexes_min.push_back(temp_vertexes_min);
        vertexes_max.push_back(temp_vertexes_max);
    }

    std::filesystem::path glb_path = file_path;

    auto streamWriter = std::make_unique<StreamWriter>(glb_path.parent_path());

    std::filesystem::path glb_file_name = glb_path.filename();

    std::unique_ptr<ResourceWriter> resourceWriter;

    resourceWriter = std::make_unique<GLBResourceWriter>(std::move(streamWriter));

    if (!resourceWriter)
    {
        throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
    }

    Document document;

    BufferBuilder bufferBuilder(std::move(resourceWriter));

    const char* bufferId = nullptr;

    bufferId = GLB_BUFFER_ID;

    bufferBuilder.AddBuffer(bufferId);

    std::vector<std::string> node_ids;

    bool bones_added = false;

    bool any_weighted = false;

    std::vector<std::string> joints;

    std::string skin_id = "";

    for (uint32_t o = 0; o < asset3ds_data.vertexes.size(); o++)
    {
        if (asset3ds_data.weighted.at(o))
        {
            any_weighted = true;
        }
    }

    if (!any_weighted)
    {
        Node node;

        node.name = asset3ds_data.root_name;

        if (rotate)
        {
            node.rotation = Quaternion(std::sqrt(2.0) / 2.0, 0.0, 0.0, -std::sqrt(2.0) / 2.0);
        }

        for (uint32_t b = 1; b < (asset3ds_data.vertexes.size() + 1); b++)
        {
            node.children.push_back(std::to_string(b));
        }

        document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty);
    }

    for (uint32_t o = 0; o < asset3ds_data.vertexes.size(); o++)
    {
        MeshPrimitive meshPrimitive;

        bufferBuilder.AddBufferView(BufferViewTarget::ELEMENT_ARRAY_BUFFER);
        meshPrimitive.indicesAccessorId = bufferBuilder.AddAccessor(asset3ds_data.indexes.at(o), { TYPE_SCALAR, COMPONENT_UNSIGNED_SHORT }).id;

        bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
        meshPrimitive.attributes["POSITION"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes.at(o), { TYPE_VEC3, COMPONENT_FLOAT, false, std::move(vertexes_min.at(o)), std::move(vertexes_max.at(o)) }).id;

        bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
        meshPrimitive.attributes["NORMAL"] = bufferBuilder.AddAccessor(asset3ds_data.normals.at(o), { TYPE_VEC3, COMPONENT_FLOAT }).id;

        //bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
        //meshPrimitive.attributes["TANGENT"] = bufferBuilder.AddAccessor(asset3ds_data.tangents.at(o), { TYPE_VEC4, COMPONENT_FLOAT }).id;

        bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
        meshPrimitive.attributes["TEXCOORD_0"] = bufferBuilder.AddAccessor(asset3ds_data.uvs.at(o), { TYPE_VEC2, COMPONENT_FLOAT }).id;

        if (asset3ds_data.colors.at(o).size() > 0)
        {
            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["COLOR_0"] = bufferBuilder.AddAccessor(asset3ds_data.colors.at(o), { TYPE_VEC4, COMPONENT_UNSIGNED_BYTE, true }).id;
        }

        if (asset3ds_data.weighted.at(o))
        {
            any_weighted = true;

            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["JOINTS_0"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes_weighted_bone_ids_0.at(o), { TYPE_VEC4, COMPONENT_UNSIGNED_BYTE }).id;

            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["JOINTS_1"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes_weighted_bone_ids_1.at(o), { TYPE_VEC4, COMPONENT_UNSIGNED_BYTE }).id;

            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["WEIGHTS_0"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes_weighted_weights_0.at(o), { TYPE_VEC4, COMPONENT_FLOAT }).id;

            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["WEIGHTS_1"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes_weighted_weights_1.at(o), { TYPE_VEC4, COMPONENT_FLOAT }).id;

            if (!bones_added)
            {
                bones_added = true;

                std::vector<Node> bones;

                for (uint32_t b = 0; b < asset3ds_data.bones_data.size(); b++)
                {
                    bones.emplace_back();

                    bones.back().name = asset3ds_data.bones_data.at(b).name;
                    bones.back().translation = Vector3(asset3ds_data.bones_positions.at(b * 8 + 4), asset3ds_data.bones_positions.at(b * 8 + 5), asset3ds_data.bones_positions.at(b * 8 + 6));
                    bones.back().rotation = Quaternion(asset3ds_data.bones_positions.at(b * 8), asset3ds_data.bones_positions.at(b * 8 + 1), asset3ds_data.bones_positions.at(b * 8 + 2), asset3ds_data.bones_positions.at(b * 8 + 3));
                }

                uint32_t node_id_offset = document.nodes.Size();

                for (uint32_t b = 0; b < asset3ds_data.bones_data.size(); b++)
                {
                    for (uint32_t a = 0; a < asset3ds_data.bones_data.size(); a++)
                    {
                        if (b == asset3ds_data.bones_data.at(a).parent_id)
                        {
                            bones.at(b).children.push_back(std::to_string(a + node_id_offset));
                        }
                    }
                }
                for (uint32_t b = 0; b < asset3ds_data.bones_data.size(); b++)
                {
                    joints.emplace_back(document.nodes.Append(bones.at(b), AppendIdPolicy::GenerateOnEmpty).id);
                }

                Node node;

                node.name = asset3ds_data.root_name;
                node.children.push_back(joints.front());

                if (rotate)
                {
                    node.rotation = Quaternion(std::sqrt(2.0) / 2.0, 0.0, 0.0, -std::sqrt(2.0) / 2.0);
                }

                node_ids.push_back(document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty).id);

                Skin temp_skin;

                temp_skin.name = "S_" + asset3ds_data.root_name;

                for (const auto& joint : joints)
                {
                    temp_skin.jointIds.push_back(joint);
                }

                bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);

                temp_skin.inverseBindMatricesAccessorId = bufferBuilder.AddAccessor(asset3ds_data.bones_inverse_matrices, { TYPE_MAT4, COMPONENT_FLOAT }).id;

                temp_skin.skeletonId = joints.front();

                skin_id = document.skins.Append(std::move(temp_skin), AppendIdPolicy::GenerateOnEmpty).id;
            }

        }

        Mesh mesh;
        mesh.primitives.push_back(std::move(meshPrimitive));
        mesh.name = asset3ds_data.names.at(o);

        auto meshId = document.meshes.Append(std::move(mesh), AppendIdPolicy::GenerateOnEmpty).id;

        Node node;
        node.meshId = meshId;

        if (asset3ds_data.weighted.at(o))
        {
            node.skinId = skin_id;
            //node.children.push_back(joints.front());
        }

        node_ids.push_back(document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty).id);
    }

    Scene scene;

    for (const auto& node_id : node_ids)
    {
        scene.nodes.push_back(node_id);
    }

    document.SetDefaultScene(std::move(scene), AppendIdPolicy::GenerateOnEmpty);

    bufferBuilder.Output(document);

    std::string manifest;

    try
    {
        manifest = Serialize(document, SerializeFlags::None);
    }
    catch (const GLTFException& ex)
    {
        std::stringstream ss;

        ss << "Microsoft::glTF::Serialize failed: ";
        ss << ex.what();

        throw std::runtime_error(ss.str());
    }

    auto& gltfResourceWriter = bufferBuilder.GetResourceWriter();

    auto glbResourceWriter = dynamic_cast<GLBResourceWriter*>(&gltfResourceWriter);

    glbResourceWriter->Flush(manifest, glb_file_name.u8string());
}

void gltf::output_to_single_file_with_textures(asset3ds& asset3ds_data, std::string& file_path, int type, bool rotate, std::vector<jpg_textures> textures)
{
    std::vector<std::vector<float>> vertexes_min;
    std::vector<std::vector<float>> vertexes_max;

    for (uint32_t a = 0; a < asset3ds_data.indexes.size(); a++)
    {
        std::vector<float> temp_vertexes_min;
        std::vector<float> temp_vertexes_max;

        for (uint32_t v = 0; v < (asset3ds_data.vertexes.at(a).size() / 3); v++)
        {
            uint32_t x = v * 3;
            uint32_t y = v * 3 + 1;
            uint32_t z = v * 3 + 2;

            if (v == 0)
            {
                temp_vertexes_min.push_back(asset3ds_data.vertexes.at(a).at(x));
                temp_vertexes_min.push_back(asset3ds_data.vertexes.at(a).at(y));
                temp_vertexes_min.push_back(asset3ds_data.vertexes.at(a).at(z));
                temp_vertexes_max.push_back(asset3ds_data.vertexes.at(a).at(x));
                temp_vertexes_max.push_back(asset3ds_data.vertexes.at(a).at(y));
                temp_vertexes_max.push_back(asset3ds_data.vertexes.at(a).at(z));
            }
            else
            {
                if (asset3ds_data.vertexes.at(a).at(x) < temp_vertexes_min.at(0))
                    temp_vertexes_min.at(0) = asset3ds_data.vertexes.at(a).at(x);
                if (asset3ds_data.vertexes.at(a).at(x) > temp_vertexes_max.at(0))
                    temp_vertexes_max.at(0) = asset3ds_data.vertexes.at(a).at(x);
                if (asset3ds_data.vertexes.at(a).at(y) < temp_vertexes_min.at(1))
                    temp_vertexes_min.at(1) = asset3ds_data.vertexes.at(a).at(y);
                if (asset3ds_data.vertexes.at(a).at(y) > temp_vertexes_max.at(1))
                    temp_vertexes_max.at(1) = asset3ds_data.vertexes.at(a).at(y);
                if (asset3ds_data.vertexes.at(a).at(z) < temp_vertexes_min.at(2))
                    temp_vertexes_min.at(2) = asset3ds_data.vertexes.at(a).at(z);
                if (asset3ds_data.vertexes.at(a).at(z) > temp_vertexes_max.at(2))
                    temp_vertexes_max.at(2) = asset3ds_data.vertexes.at(a).at(z);
            }
        }

        vertexes_min.push_back(temp_vertexes_min);
        vertexes_max.push_back(temp_vertexes_max);
    }

    std::filesystem::path glb_path = file_path;

    auto streamWriter = std::make_unique<StreamWriter>(glb_path.parent_path());

    std::filesystem::path glb_file_name = glb_path.filename();

    std::unique_ptr<ResourceWriter> resourceWriter;

    resourceWriter = std::make_unique<GLBResourceWriter>(std::move(streamWriter));

    if (!resourceWriter)
    {
        throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
    }

    Document document;

    BufferBuilder bufferBuilder(std::move(resourceWriter));

    const char* bufferId;

    bufferId = GLB_BUFFER_ID;

    bufferBuilder.AddBuffer(bufferId);

    std::vector<std::string> node_ids;

    bool bones_added = false;

    bool any_weighted = false;

    std::vector<std::string> joints;

    std::string skin_id = "";

    std::unordered_map<uint32_t, std::string> material_ids;

    for (auto& t : textures)
    {
        Material material;

        if (t.has_diffuse_texture)
        {
            Image image;
            image.bufferViewId = bufferBuilder.AddBufferView(t.texture_diffuse_jpg.data(), t.texture_diffuse_jpg.size()).id;
            image.mimeType = "image/jpeg";
            std::string image_id = document.images.Append(std::move(image), AppendIdPolicy::GenerateOnEmpty).id;

            Texture texture;
            texture.imageId = image_id;
            std::string texture_id = document.textures.Append(std::move(texture), AppendIdPolicy::GenerateOnEmpty).id;

            material.metallicRoughness.baseColorTexture.textureId = texture_id;
        }

        if (t.has_normal_texture)
        {
            Image image;
            image.bufferViewId = bufferBuilder.AddBufferView(t.texture_normal_jpg.data(), t.texture_normal_jpg.size()).id;
            image.mimeType = "image/jpeg";
            std::string image_id = document.images.Append(std::move(image), AppendIdPolicy::GenerateOnEmpty).id;

            Texture texture;
            texture.imageId = image_id;
            std::string texture_id = document.textures.Append(std::move(texture), AppendIdPolicy::GenerateOnEmpty).id;

            material.normalTexture.textureId = texture_id;
        }

        if (t.has_emissive_texture)
        {
            Image image;
            image.bufferViewId = bufferBuilder.AddBufferView(t.texture_emissive_jpg.data(), t.texture_emissive_jpg.size()).id;
            image.mimeType = "image/jpeg";
            std::string image_id = document.images.Append(std::move(image), AppendIdPolicy::GenerateOnEmpty).id;

            Texture texture;
            texture.imageId = image_id;
            std::string texture_id = document.textures.Append(std::move(texture), AppendIdPolicy::GenerateOnEmpty).id;

            material.emissiveTexture.textureId = texture_id;
        }

        material.metallicRoughness.metallicFactor = 0.0f;
        material.metallicRoughness.roughnessFactor = 0.0f;

        material_ids[t.material_id] = document.materials.Append(std::move(material), AppendIdPolicy::GenerateOnEmpty).id;
    }

    for (uint32_t o = 0; o < asset3ds_data.vertexes.size(); o++)
    {
        if (asset3ds_data.weighted.at(o))
        {
            any_weighted = true;
        }
    }

    if (!any_weighted)
    {
        Node node;

        node.name = asset3ds_data.root_name;

        if (rotate)
        {
            node.rotation = Quaternion(std::sqrt(2.0) / 2.0, 0.0, 0.0, -std::sqrt(2.0) / 2.0);
        }

        for (uint32_t b = 1; b < (asset3ds_data.vertexes.size() + 1); b++)
        {
            node.children.push_back(std::to_string(b));
        }

        document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty);
    }

    for (uint32_t o = 0; o < asset3ds_data.vertexes.size(); o++)
    {
        MeshPrimitive meshPrimitive;

        bufferBuilder.AddBufferView(BufferViewTarget::ELEMENT_ARRAY_BUFFER);
        meshPrimitive.indicesAccessorId = bufferBuilder.AddAccessor(asset3ds_data.indexes.at(o), { TYPE_SCALAR, COMPONENT_UNSIGNED_SHORT }).id;

        bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
        meshPrimitive.attributes["POSITION"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes.at(o), { TYPE_VEC3, COMPONENT_FLOAT, false, std::move(vertexes_min.at(o)), std::move(vertexes_max.at(o)) }).id;

        bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
        meshPrimitive.attributes["NORMAL"] = bufferBuilder.AddAccessor(asset3ds_data.normals.at(o), { TYPE_VEC3, COMPONENT_FLOAT }).id;

        //bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
        //meshPrimitive.attributes["TANGENT"] = bufferBuilder.AddAccessor(asset3ds_data.tangents.at(o), { TYPE_VEC4, COMPONENT_FLOAT }).id;

        bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
        meshPrimitive.attributes["TEXCOORD_0"] = bufferBuilder.AddAccessor(asset3ds_data.uvs.at(o), { TYPE_VEC2, COMPONENT_FLOAT }).id;

        if (asset3ds_data.colors.at(o).size() > 0)
        {
            //bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            //meshPrimitive.attributes["COLOR_0"] = bufferBuilder.AddAccessor(asset3ds_data.colors.at(o), { TYPE_VEC4, COMPONENT_UNSIGNED_BYTE, true }).id;
        }

        if (asset3ds_data.weighted.at(o))
        {
            any_weighted = true;

            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["JOINTS_0"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes_weighted_bone_ids_0.at(o), { TYPE_VEC4, COMPONENT_UNSIGNED_BYTE }).id;

            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["JOINTS_1"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes_weighted_bone_ids_1.at(o), { TYPE_VEC4, COMPONENT_UNSIGNED_BYTE }).id;

            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["WEIGHTS_0"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes_weighted_weights_0.at(o), { TYPE_VEC4, COMPONENT_FLOAT }).id;

            bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
            meshPrimitive.attributes["WEIGHTS_1"] = bufferBuilder.AddAccessor(asset3ds_data.vertexes_weighted_weights_1.at(o), { TYPE_VEC4, COMPONENT_FLOAT }).id;

            if (!bones_added)
            {
                bones_added = true;

                std::vector<Node> bones;

                for (uint32_t b = 0; b < asset3ds_data.bones_data.size(); b++)
                {
                    bones.emplace_back();

                    bones.back().name = asset3ds_data.bones_data.at(b).name;
                    bones.back().translation = Vector3(asset3ds_data.bones_positions.at(b * 8 + 4), asset3ds_data.bones_positions.at(b * 8 + 5), asset3ds_data.bones_positions.at(b * 8 + 6));
                    bones.back().rotation = Quaternion(asset3ds_data.bones_positions.at(b * 8), asset3ds_data.bones_positions.at(b * 8 + 1), asset3ds_data.bones_positions.at(b * 8 + 2), asset3ds_data.bones_positions.at(b * 8 + 3));
                }

                uint32_t node_id_offset = document.nodes.Size();

                for (uint32_t b = 0; b < asset3ds_data.bones_data.size(); b++)
                {
                    for (uint32_t a = 0; a < asset3ds_data.bones_data.size(); a++)
                    {
                        if (b == asset3ds_data.bones_data.at(a).parent_id)
                        {
                            bones.at(b).children.push_back(std::to_string(a + node_id_offset));
                        }
                    }
                }
                for (uint32_t b = 0; b < asset3ds_data.bones_data.size(); b++)
                {
                    joints.emplace_back(document.nodes.Append(bones.at(b), AppendIdPolicy::GenerateOnEmpty).id);
                }

                Node node;

                node.name = asset3ds_data.root_name;
                node.children.push_back(joints.front());

                if (rotate)
                {
                    node.rotation = Quaternion(std::sqrt(2.0) / 2.0, 0.0, 0.0, -std::sqrt(2.0) / 2.0);
                }

                node_ids.push_back(document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty).id);

                Skin temp_skin;

                temp_skin.name = "S_" + asset3ds_data.root_name;

                for (const auto& joint : joints)
                {
                    temp_skin.jointIds.push_back(joint);
                }

                bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);

                temp_skin.inverseBindMatricesAccessorId = bufferBuilder.AddAccessor(asset3ds_data.bones_inverse_matrices, { TYPE_MAT4, COMPONENT_FLOAT }).id;

                temp_skin.skeletonId = joints.front();

                skin_id = document.skins.Append(std::move(temp_skin), AppendIdPolicy::GenerateOnEmpty).id;
            }

        }

        std::string material_id = "";

        auto it = material_ids.find(asset3ds_data.material_ids.at(o));

        if (it != material_ids.end())
        {
            meshPrimitive.materialId = it->second;
        }

        Mesh mesh;
        mesh.primitives.push_back(std::move(meshPrimitive));
        mesh.name = asset3ds_data.names.at(o);

        auto meshId = document.meshes.Append(std::move(mesh), AppendIdPolicy::GenerateOnEmpty).id;

        Node node;
        node.meshId = meshId;

        if (asset3ds_data.weighted.at(o))
        {
            node.skinId = skin_id;
            //node.children.push_back(joints.front());
        }

        node_ids.push_back(document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty).id);
    }

    Scene scene;

    for (const auto& node_id : node_ids)
    {
        scene.nodes.push_back(node_id);
    }

    document.SetDefaultScene(std::move(scene), AppendIdPolicy::GenerateOnEmpty);

    bufferBuilder.Output(document);

    std::string manifest;

    try
    {
        manifest = Serialize(document, SerializeFlags::None);
    }
    catch (const GLTFException& ex)
    {
        std::stringstream ss;

        ss << "Microsoft::glTF::Serialize failed: ";
        ss << ex.what();

        throw std::runtime_error(ss.str());
    }

    auto& gltfResourceWriter = bufferBuilder.GetResourceWriter();

    auto glbResourceWriter = dynamic_cast<GLBResourceWriter*>(&gltfResourceWriter);

    glbResourceWriter->Flush(manifest, glb_file_name.u8string());
}
