#include "global_uniforms.h"
#include "resource/vulkan/descriptor_writer.h"

#include <array>

GlobalUniforms::GlobalUniforms(
    VkDevice device, VmaAllocator allocator, uint32_t frameCount, DescriptorAllocator& persistentAllocator
)
    : m_device(device),
      m_layout(
          device,
          std::array<VkDescriptorSetLayoutBinding, 1>{DescriptorLayout::binding(
              0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1
          )}
      )
{
    m_buffers.reserve(frameCount);
    m_descriptorSets.reserve(frameCount);
    for(uint32_t i = 0; i < frameCount; ++i)
    {
        m_buffers.emplace_back(
            allocator,
            sizeof(GlobalRenderData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU
        );

        VkDescriptorSet set = persistentAllocator.allocate(m_layout.getHandle());
        m_descriptorSets.push_back(set);

        DescriptorWriter(m_device)
            .writeBuffer(0, m_buffers[i].getBuffer(), sizeof(GlobalRenderData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .updateSet(set);
    }
}

void GlobalUniforms::update(uint32_t frameIndex, const GlobalRenderData& cpuData)
{
    assert(frameIndex < m_buffers.size() && "frame index is out of bounds!");

    void* mapped = m_buffers[frameIndex].getMappedData();
    std::memcpy(mapped, &cpuData, sizeof(GlobalRenderData));
}