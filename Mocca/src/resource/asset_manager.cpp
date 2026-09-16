#include "asset_manager.h"

#include "model_asset.h"
#include "vulkan/descriptor_writer.h"

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>

#include <glm/gtx/quaternion.hpp>

#include <stb/stb_image.h>

#include <print>

namespace
{
VkFilter extractFilter(fastgltf::Filter filter)
{
    switch(filter)
    {
    case fastgltf::Filter::Nearest:
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::NearestMipMapLinear:
        return VK_FILTER_NEAREST;
    default:
        return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode extractMipmapMode(fastgltf::Filter filter)
{
    switch(filter)
    {
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::LinearMipMapNearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}
} // namespace

AssetManager::AssetManager(
    VkDevice device,
    VkQueue graphicsQueue,
    const QueueFamilyIndices& indices,
    VmaAllocator allocator,
    VkDescriptorSetLayout materialLayout
)
    : m_device(device),
      m_allocator(allocator),
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

    m_defaultMaterial = createMaterial(MaterialPass::MainColor, defaultResources, false);
}

MaterialInstance AssetManager::createMaterial(MaterialPass pass, const MaterialResources& resources, bool doubleSided)
{
    MaterialInstance matInstance;
    matInstance.passType = pass;
    matInstance.materialSet = m_materialAllocator.allocate(m_materialLayout);
    matInstance.doubleSided = doubleSided;

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
    std::println("Loading mesh: {}", fileName.string());

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

std::shared_ptr<ModelAsset> AssetManager::loadModel(const std::filesystem::path& fileName)
{
    std::println("Loading model: {}", fileName.string());

    std::string key = fileName.string();
    auto it = m_loadedModels.find(key);
    if(it != m_loadedModels.end())
    {
        return it->second; // return chached
    }

#ifdef ASSETS_DIR
    std::filesystem::path filePath = std::filesystem::path(ASSETS_DIR) / fileName;
#else
    std::filesystem::path filePath = std::filesystem::path("assets") / fileName;
#endif

    fastgltf::GltfDataBuffer data;
    if(!data.loadFromFile(filePath))
    {
        std::println(stderr, "Failed to load glTF file: {}", filePath.string());
        return nullptr;
    }

    constexpr auto gltfOptions{
        fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble |
        fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers
    };

    fastgltf::Asset gltf;
    fastgltf::Parser parser{};

    auto type = fastgltf::determineGltfFileType(&data);
    if(type == fastgltf::GltfType::glTF)
    {
        auto load = parser.loadGltf(&data, filePath.parent_path(), gltfOptions);
        if(!load)
            return nullptr;
        gltf = std::move(load.get());
    }
    else if(type == fastgltf::GltfType::GLB)
    {
        auto load = parser.loadGltfBinary(&data, filePath.parent_path(), gltfOptions);
        if(!load)
            return nullptr;
        gltf = std::move(load.get());
    }
    else
    {
        return nullptr;
    }

    auto model = std::make_shared<ModelAsset>(m_device);


    // private descriptor pool
    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> poolSizes = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3.0f},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3.0f},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1.0f}
    };
    model->descriptorPool =
        DescriptorAllocatorGrowable(m_device, std::max(1u, static_cast<uint32_t>(gltf.materials.size())), poolSizes);


    // samplers
    for(auto& sampler : gltf.samplers)
    {
        VkSamplerCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = extractFilter(sampler.magFilter.value_or(fastgltf::Filter::Nearest)),
            .minFilter = extractFilter(sampler.minFilter.value_or(fastgltf::Filter::Nearest)),
            .mipmapMode = extractMipmapMode(sampler.minFilter.value_or(fastgltf::Filter::Nearest)),
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .minLod = 0.0f,
            .maxLod = VK_LOD_CLAMP_NONE,
        };

        VkSampler newSampler{VK_NULL_HANDLE};
        vkCreateSampler(m_device, &info, nullptr, &newSampler);
        model->samplers.push_back(newSampler);
    }

    // texture
    model->images.reserve(gltf.images.size());
    std::vector<VkImageView> gltfImageViews;
    gltfImageViews.reserve(gltf.images.size());

    for(fastgltf::Image& img : gltf.images)
    {
        auto loadedImage = loadImage(gltf, img, filePath.parent_path());
        if(loadedImage.has_value())
        {
            model->images.push_back(std::move(*loadedImage));

            gltfImageViews.push_back(model->images.back().getImageView());
        }
        else
        {
            gltfImageViews.push_back(m_errorCheckerboardTexture.getImageView());

            std::println(stderr, "Failed to load glTF image: {}", img.name);
        }
    }

    // material
    if(!gltf.materials.empty())
    {
        model->materialDataBuffer = AllocatedBuffer(
            m_allocator,
            sizeof(MaterialConstants) * gltf.materials.size(),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU
        );

        auto* sceneConstants = static_cast<MaterialConstants*>(model->materialDataBuffer.getMappedData());
        int dataIndex = 0;

        for(fastgltf::Material& mat : gltf.materials)
        {
            MaterialConstants constants{};
            constants.colorFactors = glm::vec4(
                mat.pbrData.baseColorFactor[0],
                mat.pbrData.baseColorFactor[1],
                mat.pbrData.baseColorFactor[2],
                mat.pbrData.baseColorFactor[3]
            );
            constants.metalRoughFactors =
                glm::vec4(mat.pbrData.metallicFactor, mat.pbrData.roughnessFactor, 0.0f, 0.0f);
            sceneConstants[dataIndex] = constants;

            MaterialPass passType =
                (mat.alphaMode == fastgltf::AlphaMode::Blend) ? MaterialPass::Transparent : MaterialPass::MainColor;

            MaterialResources materialResources{
                .colorImageView = m_whiteTexture.getImageView(),
                .colorSampler = m_samplerLibrary.getLinearRepeat(),
                .metalRoughImageView = m_whiteTexture.getImageView(),
                .metalRoughSampler = m_samplerLibrary.getLinearRepeat(),
                .dataBuffer = model->materialDataBuffer.getBuffer(),
                .dataBufferOffset = static_cast<uint32_t>(dataIndex * sizeof(MaterialConstants)),
            };

            if(mat.pbrData.baseColorTexture.has_value())
            {
                size_t texIdx = mat.pbrData.baseColorTexture.value().textureIndex;
                size_t imgIdx = gltf.textures[texIdx].imageIndex.value();
                materialResources.colorImageView = gltfImageViews[imgIdx];

                if(gltf.textures[texIdx].samplerIndex.has_value())
                {
                    size_t samplerIdx = gltf.textures[texIdx].samplerIndex.value();
                    materialResources.colorSampler = model->samplers[samplerIdx];
                }
            }

            if(mat.pbrData.metallicRoughnessTexture.has_value())
            {
                size_t texIdx = mat.pbrData.metallicRoughnessTexture.value().textureIndex;
                size_t imgIdx = gltf.textures[texIdx].imageIndex.value();
                materialResources.metalRoughImageView = gltfImageViews[imgIdx];

                if(gltf.textures[texIdx].samplerIndex.has_value())
                {
                    size_t samplerIdx = gltf.textures[texIdx].samplerIndex.value();
                    materialResources.metalRoughSampler = model->samplers[samplerIdx];
                }
            }

            model->materials.push_back(
                std::make_shared<MaterialInstance>(createMaterial(passType, materialResources, mat.doubleSided))
            );

            dataIndex++;
        }
    }

    // meshes
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    for(fastgltf::Mesh& mesh : gltf.meshes)
    {
        auto newMesh = std::make_shared<MeshAsset>();
        newMesh->name = mesh.name;

        indices.clear();
        vertices.clear();

        for(auto&& p : mesh.primitives)
        {
            GeoSurface newSurface{};
            newSurface.startIndex = static_cast<uint32_t>(indices.size());
            newSurface.count = static_cast<uint32_t>(gltf.accessors[p.indicesAccessor.value()].count);

            size_t initialVtx = vertices.size();

            // indices
            fastgltf::Accessor& indexAccessor = gltf.accessors[p.indicesAccessor.value()];
            indices.reserve(indices.size() + indexAccessor.count);
            fastgltf::iterateAccessor<uint32_t>(
                gltf,
                indexAccessor,
                [&](uint32_t idx) { indices.push_back(idx + static_cast<uint32_t>(initialVtx)); }
            );

            // positions
            fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
            vertices.resize(vertices.size() + posAccessor.count);
            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                gltf,
                posAccessor,
                [&](glm::vec3 v, size_t index)
                {
                    Vertex vtx{};
                    vtx.position = v;
                    vtx.normal = {1.0f, 0.0f, 0.0f};
                    vtx.color = glm::vec4{1.0f};
                    vertices[initialVtx + index] = vtx;
                }
            );

            // normals
            auto normals = p.findAttribute("NORMAL");
            if(normals != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    gltf,
                    gltf.accessors[normals->second],
                    [&](glm::vec3 v, size_t index) { vertices[initialVtx + index].normal = v; }
                );
            }

            // UV
            auto uv = p.findAttribute("TEXCOORD_0");
            if(uv != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                    gltf,
                    gltf.accessors[uv->second],
                    [&](glm::vec2 v, size_t index)
                    {
                        vertices[initialVtx + index].uv_x = v.x;
                        vertices[initialVtx + index].uv_y = v.y;
                    }
                );
            }

            // colors
            auto colors = p.findAttribute("COLOR_0");
            if(colors != p.attributes.end())
            {
                fastgltf::iterateAccessorWithIndex<glm::vec4>(
                    gltf,
                    gltf.accessors[colors->second],
                    [&](glm::vec4 v, size_t index) { vertices[initialVtx + index].color = v; }
                );
            }

            // material
            if(p.materialIndex.has_value() && p.materialIndex.value() < model->materials.size())
            {
                newSurface.material = model->materials[p.materialIndex.value()];
            }
            else
            {
                newSurface.material = model->materials.empty() ? std::make_shared<MaterialInstance>(m_defaultMaterial)
                                                               : model->materials[0];
            }

            glm::vec3 minPos = vertices[initialVtx].position;
            glm::vec3 maxPos = vertices[initialVtx].position;
            for(int i = initialVtx; i < vertices.size(); i++)
            {
                minPos = glm::min(minPos, vertices[i].position);
                maxPos = glm::max(maxPos, vertices[i].position);
            }

            newSurface.bounds.origin = (maxPos + minPos) / 2.f;
            newSurface.bounds.extents = (maxPos - minPos) / 2.f;
            newSurface.bounds.sphereRadius = glm::length(newSurface.bounds.extents);

            newSurface.count = static_cast<uint32_t>(indices.size() - newSurface.startIndex);
            newMesh->surfaces.push_back(newSurface);
        }

        newMesh->meshBuffers = m_resourceUploader.uploadMesh(indices, vertices);
        model->meshes.push_back(newMesh);
    }

    // nodes hierarchy
    for(fastgltf::Node& node : gltf.nodes)
    {
        ModelNodeDescription desc{};
        desc.name = node.name;
        desc.meshIndex =
            node.meshIndex.has_value() ? std::optional<uint32_t>(static_cast<uint32_t>(*node.meshIndex)) : std::nullopt;
        desc.children.reserve(node.children.size());
        for(auto child : node.children)
        {
            desc.children.push_back(static_cast<uint32_t>(child));
        }

        std::visit(
            fastgltf::visitor{
                [&](const fastgltf::Node::TransformMatrix& matrix)
                { std::memcpy(&desc.localTransform, matrix.data(), sizeof(matrix)); },
                [&](const fastgltf::TRS& trs)
                {
                    glm::vec3 tl(trs.translation[0], trs.translation[1], trs.translation[2]);
                    glm::quat rot(trs.rotation[3], trs.rotation[0], trs.rotation[1], trs.rotation[2]);
                    glm::vec3 sc(trs.scale[0], trs.scale[1], trs.scale[2]);

                    desc.localTransform =
                        glm::translate(glm::mat4(1.0f), tl) * glm::toMat4(rot) * glm::scale(glm::mat4(1.0f), sc);
                }
            },
            node.transform
        );


        model->nodes.push_back(desc);
    }

    m_loadedModels[key] = model;
    return model;
}

std::optional<AllocatedImage> AssetManager::loadImage(
    fastgltf::Asset& asset, fastgltf::Image& image, const std::filesystem::path& parentPath
)
{
    AllocatedImage newImage{};
    int width = 0, height = 0, nrChannels = 0;

    auto uploadHelper = [&](unsigned char* data, int w, int h)
    {
        VkExtent3D imageSize{.width = static_cast<uint32_t>(w), .height = static_cast<uint32_t>(h), .depth = 1};

        newImage =
            m_resourceUploader.uploadImage(data, imageSize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, true);
    };

    std::visit(
        fastgltf::visitor{
            [](auto& arg) {},
            [&](fastgltf::sources::URI& filePath)
            {
                assert(filePath.fileByteOffset == 0);
                assert(filePath.uri.isLocalPath());

                std::filesystem::path fullPath = parentPath / filePath.uri.path();

                unsigned char* data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 4);
                if(data)
                {
                    uploadHelper(data, width, height);
                    stbi_image_free(data);
                }
            },
            [&](fastgltf::sources::Vector& vector)
            {
                unsigned char* data = stbi_load_from_memory(
                    vector.bytes.data(),
                    static_cast<int>(vector.bytes.size()),
                    &width,
                    &height,
                    &nrChannels,
                    4
                );
                if(data)
                {
                    uploadHelper(data, width, height);
                    stbi_image_free(data);
                }
            },

            [&](fastgltf::sources::ByteView& byteView)
            {
                unsigned char* data = stbi_load_from_memory(
                    reinterpret_cast<const unsigned char*>(byteView.bytes.data()),
                    static_cast<int>(byteView.bytes.size()),
                    &width,
                    &height,
                    &nrChannels,
                    4
                );
                if(data)
                {
                    uploadHelper(data, width, height);
                    stbi_image_free(data);
                }
            },
            [&](fastgltf::sources::BufferView& view)
            {
                auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                auto& buffer = asset.buffers[bufferView.bufferIndex];

                std::visit(
                    fastgltf::visitor{
                        [](auto& arg) {},
                        [&](fastgltf::sources::Array& array)
                        {
                            const unsigned char* rawPtr =
                                reinterpret_cast<const unsigned char*>(array.bytes.data()) + bufferView.byteOffset;

                            unsigned char* data = stbi_load_from_memory(
                                rawPtr,
                                static_cast<int>(bufferView.byteLength),
                                &width,
                                &height,
                                &nrChannels,
                                4
                            );
                            if(data)
                            {
                                uploadHelper(data, width, height);
                                stbi_image_free(data);
                            }
                        },
                        [&](fastgltf::sources::Vector& vector)
                        {
                            const unsigned char* rawPtr =
                                reinterpret_cast<const unsigned char*>(vector.bytes.data()) + bufferView.byteOffset;

                            unsigned char* data = stbi_load_from_memory(
                                rawPtr,
                                static_cast<int>(bufferView.byteLength),
                                &width,
                                &height,
                                &nrChannels,
                                4
                            );
                            if(data)
                            {
                                uploadHelper(data, width, height);
                                stbi_image_free(data);
                            }
                        },
                        [&](fastgltf::sources::ByteView& byteView)
                        {
                            const unsigned char* rawPtr =
                                reinterpret_cast<const unsigned char*>(byteView.bytes.data()) + bufferView.byteOffset;

                            unsigned char* data = stbi_load_from_memory(
                                rawPtr,
                                static_cast<int>(bufferView.byteLength),
                                &width,
                                &height,
                                &nrChannels,
                                4
                            );
                            if(data)
                            {
                                uploadHelper(data, width, height);
                                stbi_image_free(data);
                            }
                        }
                    },
                    buffer.data
                );
            },
        },
        image.data
    );

    if(newImage.getImage() == VK_NULL_HANDLE)
    {
        return std::nullopt;
    }

    return newImage;
}