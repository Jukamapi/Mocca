#pragma once

#include "engine/render_feature.h"
#include "engine/renderer.h"
#include "platform/vulkan/pipeline/compute_pipeline.h"
#include "platform/vulkan/pipeline/graphics_pipeline.h"
#include "platform/vulkan/resources/descriptor_allocator.h"
#include "platform/vulkan/resources/descriptor_layout.h"
#include "resource/loader.h"


// TODO: it uses too much layers: loadShader -> resource (replace by AssetManager), directly touches m_pipeline and
// vulkan
// TODO: descriptor and pipeline mismatch
// TODO: add descriptorWriter class that simplifies descriptors writing
class TestFeature : public RenderFeature
{
public:
    TestFeature(const Renderer& renderer)
        : m_device(renderer.getContext().getLogicalDevice().getHandle()),
          m_drawExtent(renderer.getExtent())
    {

        auto binding0 = DescriptorLayout::binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
        VkDescriptorSetLayoutBinding bindings[] = {binding0};

        m_descriptorLayout = DescriptorLayout(m_device, bindings, nullptr, 0);

        DescriptorAllocator::PoolSizeRatio ratios[] = {{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f}};

        int numSets{10};
        m_descriptorAllocator = DescriptorAllocator(m_device, numSets, ratios);

        auto compCode = loadShader("shader.comp.spv");

        m_pipeline = ComputePipeline{m_device, m_descriptorLayout, compCode};
    }

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override
    {

        if(frameIndex >= m_descriptorSets.size())
        {
            size_t oldSize = m_descriptorSets.size();
            m_descriptorSets.resize(frameIndex + 1, VK_NULL_HANDLE);

            for(size_t i = oldSize; i < m_descriptorSets.size(); i++)
            {
                m_descriptorSets[i] = m_descriptorAllocator.allocate(m_descriptorLayout.getHandle());
            }
        }

        VkDescriptorSet currentSet = m_descriptorSets[frameIndex];

        VkDescriptorImageInfo imgInfo{
            .imageView = drawImageView,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
        };

        VkWriteDescriptorSet drawImageWrite{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = currentSet,
            .dstBinding = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &imgInfo,
        };

        // TODO: bindless approach or descriptor set per frame
        vkUpdateDescriptorSets(m_device, 1, &drawImageWrite, 0, nullptr);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline.getHandle());

        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            m_pipeline.getLayout(),
            0,
            1,
            &currentSet,
            0,
            nullptr
        );

        vkCmdDispatch(cmd, std::ceil(m_drawExtent.width / 16.0), std::ceil(m_drawExtent.height / 16.0), 1);

        // vkCmdDraw(cmd, 3, 1, 0, 0);
    }


private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkImageView m_drawImageView{VK_NULL_HANDLE};
    ComputePipeline m_pipeline;
    DescriptorLayout m_descriptorLayout;
    DescriptorAllocator m_descriptorAllocator;
    std::vector<VkDescriptorSet> m_descriptorSets;
    VkExtent2D m_drawExtent{};
};