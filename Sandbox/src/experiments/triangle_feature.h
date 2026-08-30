#pragma once

#include "renderer/pipeline/graphics_pipeline.h"
#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/render_feature.h"
#include "renderer/renderer.h"
#include "resource/loader.h"

#include <imgui.h>


class TriangleFeature : public RenderFeature
{
public:
    TriangleFeature(Renderer& renderer)
        : m_device(renderer.getContext().getLogicalDevice().getHandle()),
          m_drawExtent(renderer.getExtent())
    {

        auto vertShader = loadShader("colored_triangle.vert.spv");
        auto fragShader = loadShader("colored_triangle.frag.spv");

        auto& pipelineManager = renderer.getPipelineManager();

        m_trianglePipeline = &pipelineManager.createGraphicsPipeline(
            "triangle",
            {
                renderer.getDrawFormat(),
                renderer.getDepthFormat(),
                vertShader,
                fragShader,
            }
        );
    }

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_trianglePipeline->getHandle());
        vkCmdDraw(cmd, 3, 1, 0, 0);
    }

    void onResize(uint32_t width, uint32_t height) override
    {
        m_drawExtent = {width, height};
    }

    RenderPassType getType() const override
    {
        return RenderPassType::Graphics;
    }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkImageView m_drawImageView{VK_NULL_HANDLE};

    GraphicsPipeline* m_trianglePipeline;

    VkExtent2D m_drawExtent{};
};