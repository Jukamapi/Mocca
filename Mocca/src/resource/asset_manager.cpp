#include "asset_manager.h"

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include <print>

AssetManager::AssetManager(
    VkDevice device, VkQueue graphicsQueue, const QueueFamilyIndices& indices, VmaAllocator allocator
)
    : m_resourceUploader(device, graphicsQueue, indices, allocator)
{
}


std::optional<std::vector<std::shared_ptr<MeshAsset>>> AssetManager::loadGltfMeshes(std::filesystem::path filePath)
{
    std::cout << "Loading GLTF: " << filePath << std::endl;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(filePath);

    constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

    fastgltf::Asset gltf;
    fastgltf::Parser parser{};

    auto load = parser.loadGltfBinary(&data, filePath.parent_path(), gltfOptions);

    if(load)
    {
        gltf = std::move(load.get());
    }
    else
    {
        std::println("Failed to load glTF: {} \n", fastgltf::to_underlying(load.error()));
        return {};
    }

    std::vector<std::shared_ptr<MeshAsset>> meshes;

    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    for(fastgltf::Mesh& mesh : gltf.meshes)
    {
        MeshAsset newMesh;

        newMesh.name = mesh.name;

        indices.clear();
        vertices.clear();

        for(auto&& p : mesh.primitives)
        {
            GeoSurface newSurface;
            newSurface.startIndex = static_cast<uint32_t>(indices.size());
            newSurface.count = static_cast<uint32_t>(gltf.accessors[p.indicesAccessor.value()].count);

            size_t initialVertex = vertices.size();

            // loading index
            {
                fastgltf::Accessor& indexAccessor = gltf.accessors[p.indicesAccessor.value()];

                indices.reserve(indices.size() + indexAccessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(
                    gltf,
                    indexAccessor,
                    [&](std::uint32_t idx) { indices.push_back(idx + initialVertex); }
                );
            }

            // load vertex positions
            {
                fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
                vertices.resize(vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    gltf,
                    posAccessor,
                    [&](glm::vec3 v, size_t index)
                    {
                        Vertex newvtx;
                        newvtx.position = v;
                        newvtx.normal = {1, 0, 0};
                        newvtx.color = glm::vec4{1.f};
                        newvtx.uv_x = 0;
                        newvtx.uv_y = 0;
                        vertices[initialVertex + index] = newvtx;
                    }
                );
            }

            // load normals
            auto normals = p.findAttribute("NORMAL");
            if(normals != p.attributes.end())
            {

                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    gltf,
                    gltf.accessors[(*normals).second],
                    [&](glm::vec3 v, size_t index) { vertices[initialVertex + index].normal = v; }
                );
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if(uv != p.attributes.end())
            {

                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                    gltf,
                    gltf.accessors[(*uv).second],
                    [&](glm::vec2 v, size_t index)
                    {
                        vertices[initialVertex + index].uv_x = v.x;
                        vertices[initialVertex + index].uv_y = v.y;
                    }
                );
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if(colors != p.attributes.end())
            {

                fastgltf::iterateAccessorWithIndex<glm::vec4>(
                    gltf,
                    gltf.accessors[(*colors).second],
                    [&](glm::vec4 v, size_t index) { vertices[initialVertex + index].color = v; }
                );
            }

            newMesh.surfaces.push_back(newSurface);
        }

        if(OVERRIDE_COLORS)
        {
            for(Vertex& vtx : vertices)
            {
                vtx.color = glm::vec4(vtx.normal, 1.f);
            }
        }
        newMesh.meshBuffers = m_resourceUploader.uploadMesh(indices, vertices);

        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newMesh)));
    }

    return meshes;
}


void AssetManager::initDefaultData()
{
    std::array<Vertex, 4> rectVertices;

    rectVertices[0].position = {0.5, -0.5, 0};
    rectVertices[1].position = {0.5, 0.5, 0};
    rectVertices[2].position = {-0.5, -0.5, 0};
    rectVertices[3].position = {-0.5, 0.5, 0};

    rectVertices[0].color = {0, 0, 0, 1};
    rectVertices[1].color = {0.5, 0.5, 0.5, 1};
    rectVertices[2].color = {1, 0, 0, 1};
    rectVertices[3].color = {0, 1, 0, 1};

    std::array<uint32_t, 6> rectIndices;

    rectIndices[0] = 0;
    rectIndices[1] = 1;
    rectIndices[2] = 2;

    rectIndices[3] = 2;
    rectIndices[4] = 1;
    rectIndices[5] = 3;

    m_rectangle = m_resourceUploader.uploadMesh(rectIndices, rectVertices);
}