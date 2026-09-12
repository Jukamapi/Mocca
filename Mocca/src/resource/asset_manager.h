#pragma once

#include "mesh_types.h"
#include "platform/vk_device_types.h"
#include "resource_uploader.h"
#include "vulkan/allocated_image.h"
#include "vulkan/descriptor_allocator_growable.h"
#include "vulkan/sampler_library.h"


#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

class AssetManager
{
public:
    AssetManager(
        VkDevice device,
        VkQueue graphicsQueue,
        const QueueFamilyIndices& indices,
        VmaAllocator allocator,
        VkDescriptorSetLayout materialLayout
    );
    ~AssetManager() = default;

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    AssetManager(AssetManager&&) noexcept = default;
    AssetManager& operator=(AssetManager&&) noexcept = default;


    std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(std::filesystem::path filePath);

    void initDefaultTextures();

    MaterialInstance createMaterial(MaterialPass pass, const MaterialResources& resources);

    const AllocatedImage& getErrorCheckerboardImage() const
    {
        return m_errorCheckerboardTexture;
    }
    const AllocatedImage& getWhiteImage() const
    {
        return m_whiteTexture;
    }
    const AllocatedImage& getBlackImage() const
    {
        return m_blackTexture;
    }
    const AllocatedImage& getGreyImage() const
    {
        return m_greyTexture;
    }
    MaterialInstance& getDefaultMaterial()
    {
        return m_defaultMaterial;
    }
    SamplerLibrary& getSamplerLibrary()
    {
        return m_samplerLibrary;
    }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkDescriptorSetLayout m_materialLayout{VK_NULL_HANDLE};

    SamplerLibrary m_samplerLibrary;
    ResourceUploader m_resourceUploader;
    DescriptorAllocatorGrowable m_materialAllocator;

    // fallback material
    AllocatedBuffer m_defaultMaterialConstants;
    MaterialInstance m_defaultMaterial;

    // fallback textures
    AllocatedImage m_whiteTexture;
    AllocatedImage m_blackTexture;
    AllocatedImage m_greyTexture;
    AllocatedImage m_errorCheckerboardTexture;

    constexpr static bool OVERRIDE_COLORS = false;
};