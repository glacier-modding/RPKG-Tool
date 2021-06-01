#include "gltf.h"
#include "file.h"
#include "util.h"
#include <GLTFSDK/BufferBuilder.h>
#include <GLTFSDK/IStreamWriter.h>
#include <GLTFSDK/GLBResourceWriter.h>
#include <GLTFSDK/Serialize.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <assert.h>

using namespace Microsoft::glTF;

class StreamWriter : public IStreamWriter
{
public:
    StreamWriter(std::filesystem::path pathBase) : m_pathBase(std::move(pathBase))
    {
        assert(m_pathBase.has_root_path());
    }

    std::shared_ptr<std::ostream> GetOutputStream(const std::string& filename) const override
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

void gltf::output_to_single_file(asset3ds& asset3ds_data, std::string& file_path, int type)
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

        node.rotation = Quaternion(std::sqrt(2.0) / 2.0, 0.0, 0.0, -std::sqrt(2.0) / 2.0);

        for (uint32_t b = 1; b < (asset3ds_data.vertexes.size() + 1); b++)
        {
            node.children.push_back(std::to_string(b));
        }

        document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty).id;
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
                node.rotation = Quaternion(std::sqrt(2.0) / 2.0, 0.0, 0.0, -std::sqrt(2.0) / 2.0);

                node_ids.push_back(document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty).id);

                Skin temp_skin;

                temp_skin.name = "S_" + asset3ds_data.root_name;

                for (uint32_t b = 0; b < joints.size(); b++)
                {
                    temp_skin.jointIds.push_back(joints.at(b));
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

    for (uint32_t b = 0; b < node_ids.size(); b++)
    {
        scene.nodes.push_back(node_ids.at(b));
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

/*#include "gltf.h"
#include "file.h"
#include "util.h"
#include <GLTFSDK/BufferBuilder.h>
#include <GLTFSDK/Serialize.h>
#include <GLTFSDK/IStreamWriter.h>
#include <GLTFSDK/GLTFResourceWriter.h>
#include <GLTFSDK/GLBResourceWriter.h>
#include <GLTFSDK/ExtensionsKHR.h>
#include <GLTFSDK/Validation.h>
#include <GLTFSDK/AnimationUtils.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <assert.h>

using namespace Microsoft::glTF;

class StreamWriter : public IStreamWriter
{
public:
    StreamWriter(std::filesystem::path pathBase) : m_pathBase(std::move(pathBase))
    {
        assert(m_pathBase.has_root_path());
    }

    // Resolves the relative URIs of any external resources declared in the glTF manifest
    std::shared_ptr<std::ostream> GetOutputStream(const std::string& filename) const override
    {
        // In order to construct a valid stream:
        // 1. The filename argument will be encoded as UTF-8 so use filesystem::u8path to
        //    correctly construct a path instance.
        // 2. Generate an absolute path by concatenating m_pathBase with the specified filename
        //    path. The filesystem::operator/ uses the platform's preferred directory separator
        //    if appropriate.
        // 3. Always open the file stream in binary mode. The glTF SDK will handle any text
        //    encoding issues for us.
        auto streamPath = m_pathBase / std::filesystem::u8path(filename);
        auto stream = std::make_shared<std::ofstream>(streamPath, std::ios_base::binary);

        // Check if the stream has no errors and is ready for I/O operations
        if (!stream || !(*stream))
        {
            throw std::runtime_error("Unable to create a valid output stream for uri: " + filename);
        }

        return stream;
    }

private:
    std::filesystem::path m_pathBase;
};

void CreateTriangleResources(Document& document, BufferBuilder& bufferBuilder, std::string& accessorIdIndices, std::string& accessorIdPositions)
{
    // Create all the resource data (e.g. triangle indices and
    // vertex positions) that will be written to the binary buffer
    const char* bufferId = nullptr;

    // Specify the 'special' GLB buffer ID. This informs the GLBResourceWriter that it should use
    // the GLB container's binary chunk (usually the desired buffer location when creating GLBs)
    if (dynamic_cast<const GLBResourceWriter*>(&bufferBuilder.GetResourceWriter()))
    {
        bufferId = GLB_BUFFER_ID;
    }

    // Create a Buffer - it will be the 'current' Buffer that all the BufferViews
    // created by this BufferBuilder will automatically reference
    bufferBuilder.AddBuffer(bufferId);

    // Create a BufferView with a target of ELEMENT_ARRAY_BUFFER (as it will reference index
    // data) - it will be the 'current' BufferView that all the Accessors created by this
    // BufferBuilder will automatically reference
    bufferBuilder.AddBufferView(BufferViewTarget::ELEMENT_ARRAY_BUFFER);

    // Add an Accessor for the indices
    std::vector<uint16_t> indices = {
        0U, 1U, 2U
    };

    // Copy the Accessor's id - subsequent calls to AddAccessor may invalidate the returned reference
    accessorIdIndices = bufferBuilder.AddAccessor(indices, { TYPE_SCALAR, COMPONENT_UNSIGNED_SHORT }).id;

    // Create a BufferView with target ARRAY_BUFFER (as it will reference vertex attribute data)
    bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);

    // Add an Accessor for the positions
    std::vector<float> positions = {
        0.0f, 0.0f, 0.0f, // Vertex 0
        1.0f, 0.0f, 0.0f, // Vertex 1
        0.0f, 1.0f, 0.0f  // Vertex 2
    };

    std::vector<float> minValues(3U, std::numeric_limits<float>::max());
    std::vector<float> maxValues(3U, std::numeric_limits<float>::lowest());

    const size_t positionCount = positions.size();

    // Accessor min/max properties must be set for vertex position data so calculate them here
    for (size_t i = 0U, j = 0U; i < positionCount; ++i, j = (i % 3U))
    {
        minValues[j] = std::min(positions[i], minValues[j]);
        maxValues[j] = std::max(positions[i], maxValues[j]);
    }

    accessorIdPositions = bufferBuilder.AddAccessor(positions, { TYPE_VEC3, COMPONENT_FLOAT, false, std::move(minValues), std::move(maxValues) }).id;

    // Add all of the Buffers, BufferViews and Accessors that were created using BufferBuilder to
    // the Document. Note that after this point, no further calls should be made to BufferBuilder
    bufferBuilder.Output(document);
}

void CreateTriangleEntities(Document& document, const std::string& accessorIdIndices, const std::string& accessorIdPositions)
{
    // Create a very simple glTF Document with the following hierarchy:
    //  Scene
    //     Node
    //       Mesh (Triangle)
    //         MeshPrimitive
    //           Material (Blue)
    // 
    // A Document can be constructed top-down or bottom up. However, if constructed top-down
    // then the IDs of child entities must be known in advance, which prevents using the glTF
    // SDK's automatic ID generation functionality.

    // Construct a Material
    Material material;
    material.metallicRoughness.baseColorFactor = Color4(0.0f, 0.0f, 1.0f, 1.0f);
    material.metallicRoughness.metallicFactor = 0.2f;
    material.metallicRoughness.roughnessFactor = 0.4f;
    material.doubleSided = true;

    // Add it to the Document and store the generated ID
    auto materialId = document.materials.Append(std::move(material), AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a MeshPrimitive. Unlike most types in glTF, MeshPrimitives are direct children
    // of their parent Mesh entity rather than being children of the Document. This is why they
    // don't have an ID member.
    MeshPrimitive meshPrimitive;
    meshPrimitive.materialId = materialId;
    meshPrimitive.indicesAccessorId = accessorIdIndices;
    meshPrimitive.attributes[ACCESSOR_POSITION] = accessorIdPositions;

    // Construct a Mesh and add the MeshPrimitive as a child
    Mesh mesh;
    mesh.primitives.push_back(std::move(meshPrimitive));
    // Add it to the Document and store the generated ID
    auto meshId = document.meshes.Append(std::move(mesh), AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a Node adding a reference to the Mesh
    Node node;
    node.meshId = meshId;
    // Add it to the Document and store the generated ID
    auto nodeId = document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a Scene
    Scene scene;
    scene.nodes.push_back(nodeId);
    // Add it to the Document, using a utility method that also sets the Scene as the Document's default
    document.SetDefaultScene(std::move(scene), AppendIdPolicy::GenerateOnEmpty);
}

void gltf::output_to_single_file(asset3ds& asset3ds_data, std::string& file_path, int type)
{
    std::filesystem::path path = "R:\\test.glb";

    // Pass the absolute path, without the filename, to the stream writer
    auto streamWriter = std::make_unique<StreamWriter>(path.parent_path());

    std::filesystem::path pathFile = path.filename();
    std::filesystem::path pathFileExt = pathFile.extension();

    auto MakePathExt = [](const std::string& ext)
    {
        return "." + ext;
    };

    std::unique_ptr<ResourceWriter> resourceWriter;

    // If the file has a '.gltf' extension then create a GLTFResourceWriter
    if (pathFileExt == MakePathExt(GLTF_EXTENSION))
    {
        resourceWriter = std::make_unique<GLTFResourceWriter>(std::move(streamWriter));
    }

    // If the file has a '.glb' extension then create a GLBResourceWriter. This class derives
    // from GLTFResourceWriter and adds support for writing manifests to a GLB container's
    // JSON chunk and resource data to the binary chunk.
    if (pathFileExt == MakePathExt(GLB_EXTENSION))
    {
        resourceWriter = std::make_unique<GLBResourceWriter>(std::move(streamWriter));
    }

    if (!resourceWriter)
    {
        throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
    }

    // The Document instance represents the glTF JSON manifest
    Document document;

    std::string accessorIdIndices;
    std::string accessorIdPositions;

    // Use the BufferBuilder helper class to simplify the process of
    // constructing valid glTF Buffer, BufferView and Accessor entities
    BufferBuilder bufferBuilder(std::move(resourceWriter));

    CreateTriangleResources(document, bufferBuilder, accessorIdIndices, accessorIdPositions);
    CreateTriangleEntities(document, accessorIdIndices, accessorIdPositions);

    std::string manifest;

    try
    {
        // Serialize the glTF Document into a JSON manifest
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

    if (auto glbResourceWriter = dynamic_cast<GLBResourceWriter*>(&gltfResourceWriter))
    {
        glbResourceWriter->Flush(manifest, pathFile.u8string()); // A GLB container isn't created until the GLBResourceWriter::Flush member function is called
    }
    else
    {
        gltfResourceWriter.WriteExternal(pathFile.u8string(), manifest); // Binary resources have already been written, just need to write the manifest
    }
}*/