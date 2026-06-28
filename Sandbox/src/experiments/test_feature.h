#pragma once

#include "engine/renderer/render_feature.h"
#include "engine/renderer/renderer.h"
#include "platform/vulkan/pipeline/graphics_pipeline.h"
#include "platform/vulkan/resources/descriptor_allocator.h"
#include "platform/vulkan/resources/descriptor_layout.h"
#include "resource/loader.h"


// TODO: it uses too much layers: loadShader -> resource (replace by AssetManager), directly touches m_pipeline and
// vulkan
class TestFeature : public RenderFeature
{
public:
    TestFeature(const Renderer& renderer)
        : m_device(renderer.getContext().getLogicalDevice().getHandle())
    {

        auto binding0 = DescriptorLayout::binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
        VkDescriptorSetLayoutBinding bindings[] = {binding0};

        m_descriptorLayout = DescriptorLayout(m_device, bindings, nullptr, VK_SHADER_STAGE_COMPUTE_BIT);

        DescriptorAllocator::PoolSizeRatio ratios[] = {{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f}};
        m_descriptorAllocator = DescriptorAllocator(m_device, 10, ratios);

        m_descriptorSet = m_descriptorAllocator.allocate(m_descriptorLayout.getHandle());

        auto compCode = loadShader("shader.comp.spv");

        // m_pipeline = ComputePipeline();
    }

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override
    {

        VkDescriptorImageInfo imgInfo{
            .imageView = drawImageView,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
        };

        VkWriteDescriptorSet drawImageWrite{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = m_descriptorSet,
            .dstBinding = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &imgInfo,
        };

        // TODO: bindless approach or descriptor set per frame
        vkUpdateDescriptorSets(m_device, 1, &drawImageWrite, 0, nullptr);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getHandle());

        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline.getLayout(),
            0,
            1,
            &m_descriptorSet,
            0,
            nullptr
        );

        vkCmdDraw(cmd, 3, 1, 0, 0);
    }


private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkImageView m_drawImageView{VK_NULL_HANDLE};
    GraphicsPipeline m_pipeline;
    DescriptorLayout m_descriptorLayout;
    DescriptorAllocator m_descriptorAllocator;
    VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};
};