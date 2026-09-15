#pragma once

#include "renderer/render_feature.h"

#include <imgui.h>

class GraphicsPipeline;
class Renderer;

class TriangleFeature : public RenderFeature
{
public:
    TriangleFeature(Renderer& renderer);

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override;

    void onResize(uint32_t width, uint32_t height) override;

    RenderPassType getType() const override;

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkImageView m_drawImageView{VK_NULL_HANDLE};

    GraphicsPipeline* m_trianglePipeline;

    VkExtent2D m_drawExtent{};
};