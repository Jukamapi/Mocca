#pragma once

#include "resource/vulkan/allocated_buffer.h"
#include "resource/vulkan/descriptor_allocator.h"
#include "resource/vulkan/descriptor_layout.h"

#include <volk.h>
#include <glm/mat4x4.hpp>

#include <vector>

struct GlobalRenderData
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambientColor;
    glm::vec4 sunlightDirection;
    glm::vec4 sunlightColor;
};

class GlobalUniforms
{
public:
    GlobalUniforms(
        VkDevice device, VmaAllocator allocator, uint32_t frameCount, DescriptorAllocator& persistentAllocator
    );
    ~GlobalUniforms() = default;

    GlobalUniforms(const GlobalUniforms&) = delete;
    GlobalUniforms& operator=(const GlobalUniforms&) = delete;
    GlobalUniforms(GlobalUniforms&&) noexcept = default;
    GlobalUniforms& operator=(GlobalUniforms&&) noexcept = default;


    void update(uint32_t frameIndex, const GlobalRenderData& cpuData);

    VkDescriptorSetLayout getLayout() const
    {
        return m_layout.getHandle();
    }
    VkDescriptorSet getDescriptorSet(uint32_t frameIndex) const
    {
        return m_descriptorSets[frameIndex];
    }


private:
    VkDevice m_device{VK_NULL_HANDLE};
    DescriptorLayout m_layout;

    std::vector<AllocatedBuffer> m_buffers;
    std::vector<VkDescriptorSet> m_descriptorSets;
};