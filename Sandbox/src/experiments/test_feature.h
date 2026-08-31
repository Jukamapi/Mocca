#pragma once

#include "core/vk_types.h"

#include "renderer/pipelines/compute_pipeline.h"
#include "renderer/pipelines/pipeline_manager.h"
#include "renderer/render_feature.h"
#include "renderer/renderer.h"
#include "resource/loader.h"
#include "resource/vulkan/descriptor_allocator.h"
#include "resource/vulkan/descriptor_layout.h"


#include <imgui.h>


class TestFeature : public RenderFeature
{
public:
    TestFeature(Renderer& renderer)
        : m_device(renderer.getContext().getLogicalDevice().getHandle()),
          m_drawExtent(renderer.getExtent())
    {

        auto binding0 = DescriptorLayout::binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
        VkDescriptorSetLayoutBinding bindings[] = {binding0};

        m_descriptorLayout = DescriptorLayout(m_device, bindings, nullptr, 0);

        DescriptorAllocator::PoolSizeRatio ratios[] = {{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f}};

        int numSets{10};
        m_descriptorAllocator = DescriptorAllocator(m_device, numSets, ratios);

        auto gradientShader = loadShader("gradient_color.comp.spv");
        auto skyShader = loadShader("sky.comp.spv");

        auto& pipelineManager = renderer.getPipelineManager();

        m_skyPipeline = &pipelineManager.createComputePipeline("sky", m_descriptorLayout, skyShader);
        m_gradientPipeline = &pipelineManager.createComputePipeline("gradient", m_descriptorLayout, gradientShader);

        // gradient first --------------------------------------------
        ComputeEffect gradient;
        gradient.pipeline = m_gradientPipeline;
        gradient.name = "gradient";
        gradient.data = {};

        gradient.data.data1 = glm::vec4(1, 0, 0, 1);
        gradient.data.data2 = glm::vec4(0, 0, 1, 1);

        m_backgroundEffects.push_back(gradient);


        // sky ------------------------------------------------
        ComputeEffect sky;
        sky.pipeline = m_skyPipeline;
        sky.name = "sky";
        sky.data = {};

        sky.data.data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

        m_backgroundEffects.push_back(sky);
    }

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override
    {

        // TODO: very inneficient with this "if" and allocating on hotpath
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

        const ComputeEffect& activeEffect = m_backgroundEffects[m_currentBackgroundEffect];

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, activeEffect.pipeline->getHandle());

        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            activeEffect.pipeline->getLayout(),
            0,
            1,
            &currentSet,
            0,
            nullptr
        );

        vkCmdPushConstants(
            cmd,
            activeEffect.pipeline->getLayout(),
            VK_SHADER_STAGE_COMPUTE_BIT,
            0,
            sizeof(ComputePushConstants),
            &m_backgroundEffects[m_currentBackgroundEffect].data
        );

        vkCmdDispatch(cmd, std::ceil(m_drawExtent.width / 16.0), std::ceil(m_drawExtent.height / 16.0), 1);
    }

    void onImgui() override
    {
        if(ImGui::Begin("background"))
        {

            ComputeEffect& selected = m_backgroundEffects[m_currentBackgroundEffect];

            ImGui::Text("Selected effect: %s", selected.name);

            ImGui::SliderInt("Effect Index", &m_currentBackgroundEffect, 0, m_backgroundEffects.size() - 1);

            ImGui::InputFloat4("data1", (float*)&selected.data.data1);
            ImGui::InputFloat4("data2", (float*)&selected.data.data2);
            ImGui::InputFloat4("data3", (float*)&selected.data.data3);
            ImGui::InputFloat4("data4", (float*)&selected.data.data4);
        }
        ImGui::End();
    }

    void onResize(uint32_t width, uint32_t height) override
    {
        m_drawExtent = {width, height};
    }

    RenderPassType getType() const override
    {
        return RenderPassType::Compute;
    }

private:
    VkDevice m_device{VK_NULL_HANDLE};

    // doing it this way so they are non-owning
    ComputePipeline* m_gradientPipeline;
    ComputePipeline* m_skyPipeline;

    DescriptorLayout m_descriptorLayout;
    DescriptorAllocator m_descriptorAllocator;
    std::vector<VkDescriptorSet> m_descriptorSets;
    VkExtent2D m_drawExtent{};

    std::vector<ComputeEffect> m_backgroundEffects;
    int m_currentBackgroundEffect{0};
};