#include "asset_manager.h"

#include "resource/vulkan/descriptor_writer.h"

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include <iostream>
#include <print>

AssetManager::AssetManager(
    VkDevice device,
    VkQueue graphicsQueue,
    const QueueFamilyIndices& indices,
    VmaAllocator allocator,
    VkDescriptorSetLayout materialLayout
)
    : m_device(device),
      m_materialLayout(materialLayout),
      m_samplerLibrary(device),
      m_resourceUploader(device, graphicsQueue, indices, allocator),
      m_materialAllocator(
          device,
          64,
          std::array{
              DescriptorAllocatorGrowable::PoolSizeRatio{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.0f},
              DescriptorAllocatorGrowable::PoolSizeRatio{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2.0f}
          }
      )
{
    initDefaultTextures();

    m_defaultMaterialConstants = AllocatedBuffer(
        allocator,
        sizeof(MaterialConstants),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU
    );

    auto* constants = static_cast<MaterialConstants*>(m_defaultMaterialConstants.getMappedData());
    constants->colorFactors = glm::vec4(1.0f);
    constants->metalRoughFactors = glm::vec4(1.0f, 0.5f, 0.0f, 0.0f);

    MaterialResources defaultResources{
        .colorImageView = m_whiteTexture.getImageView(),
        .colorSampler = m_samplerLibrary.getLinearRepeat(),
        .metalRoughImageView = m_whiteTexture.getImageView(),
        .metalRoughSampler = m_samplerLibrary.getLinearRepeat(),
        .dataBuffer = m_defaultMaterialConstants.getBuffer(),
        .dataBufferOffset = 0,
    };

    m_defaultMaterial = createMaterial(MaterialPass::MainColor, defaultResources);
}

MaterialInstance AssetManager::createMaterial(MaterialPass pass, const MaterialResources& resources)
{
    MaterialInstance matInstance;
    matInstance.passType = pass;
    matInstance.materialSet = m_materialAllocator.allocate(m_materialLayout);

    DescriptorWriter(m_device)
        .writeBuffer(
            0,
            resources.dataBuffer,
            sizeof(MaterialConstants),
            resources.dataBufferOffset,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        )
        .writeImage(
            1,
            resources.colorImageView,
            resources.colorSampler,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
        )
        .writeImage(
            2,
            resources.metalRoughImageView,
            resources.metalRoughSampler,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
        )
        .updateSet(matInstance.materialSet);

    return matInstance;
}

void AssetManager::initDefaultTextures()
{
    uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
    m_whiteTexture =
        m_resourceUploader.uploadImage(&white, {1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    uint32_t grey = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
    m_greyTexture =
        m_resourceUploader.uploadImage(&grey, {1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
    m_blackTexture =
        m_resourceUploader.uploadImage(&black, {1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);


    std::array<uint32_t, 16 * 16> pixels;
    uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
    for(int x = 0; x < 16; x++)
    {
        for(int y = 0; y < 16; y++)
        {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }

    m_errorCheckerboardTexture =
        m_resourceUploader
            .uploadImage(pixels.data(), {16, 16, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
}

std::optional<std::vector<std::shared_ptr<MeshAsset>>> AssetManager::loadGltfMeshes(std::filesystem::path fileName)
{
    std::cout << "Loading GLTF: " << fileName << std::endl;

#ifdef ASSETS_DIR
    std::filesystem::path filePath = std::filesystem::path(ASSETS_DIR) / fileName;
#else
    std::filesystem::path filePath = std::filesystem::path("assets") / fileName;
#endif

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
