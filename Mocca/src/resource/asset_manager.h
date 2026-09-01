#pragma once

#include "core/vk_types.h"
#include "resource_uploader.h"


#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

class AssetManager
{
public:
    AssetManager(VkDevice device, VkQueue graphicsQueue, const QueueFamilyIndices& indices, VmaAllocator allocator);

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    AssetManager(AssetManager&&) noexcept = default;
    AssetManager& operator=(AssetManager&&) noexcept = default;


    std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(std::filesystem::path filePath);

private:
    ResourceUploader m_resourceUploader;

    constexpr static bool OVERRIDE_COLORS = true;
};