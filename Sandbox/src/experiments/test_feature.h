#pragma once

#include "renderer/compute_types.h"
#include "renderer/pipelines/compute_pipeline.h"
#include "renderer/render_feature.h"
#include "resource/vulkan/descriptor_allocator.h"
#include "resource/vulkan/descriptor_layout.h"


#include <imgui.h>

class Renderer;

class TestFeature : public RenderFeature
{
public:
    TestFeature(Renderer& renderer);

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override;

    void onImgui() override;

    void onResize(uint32_t width, uint32_t height) override;

    RenderPassType getType() const override;

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