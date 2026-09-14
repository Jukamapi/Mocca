#pragma once

#include "mesh_types.h"
#include "vulkan/allocated_buffer.h"
#include "vulkan/allocated_image.h"
#include "vulkan/descriptor_allocator_growable.h"


#include <vulkan/vulkan.h>
#include <glm/mat4x4.hpp>


#include <memory>
#include <optional>
#include <string>
#include <vector>

struct ModelNodeDescription
{
    std::string name;
    glm::mat4 localTransform{1.0f};
    std::optional<uint32_t> meshIndex;
    std::vector<uint32_t> children;
};

class ModelAsset
{
public:
    explicit ModelAsset(VkDevice device)
        : m_device(device)
    {
    }

    ~ModelAsset()
    {
        for(VkSampler sampler : samplers)
        {
            vkDestroySampler(m_device, sampler, nullptr);
        }
    }

    std::shared_ptr<MeshAsset> getMesh(const std::string& name) const
    {
        for(const auto& mesh : meshes)
        {
            if(mesh->name == name)
                return mesh;
        }
        return nullptr;
    }

    ModelAsset(const ModelAsset&) = delete;
    ModelAsset& operator=(const ModelAsset&) = delete;
    ModelAsset(ModelAsset&&) noexcept = default;
    ModelAsset& operator=(ModelAsset&&) noexcept = default;

    std::vector<std::shared_ptr<MeshAsset>> meshes;
    std::vector<std::shared_ptr<MaterialInstance>> materials;
    std::vector<ModelNodeDescription> nodes;
    std::vector<AllocatedImage> images;

    AllocatedBuffer materialDataBuffer;
    DescriptorAllocatorGrowable descriptorPool;

    std::vector<VkSampler> samplers;

private:
    VkDevice m_device{VK_NULL_HANDLE};
};