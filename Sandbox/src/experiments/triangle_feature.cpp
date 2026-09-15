#include "triangle_feature.h"

#include "renderer/renderer.h"

#include "renderer/pipelines/graphics_pipeline.h"
#include "renderer/pipelines/pipeline_manager.h"
#include "resource/loader.h"

TriangleFeature::TriangleFeature(Renderer& renderer)
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

void TriangleFeature::onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex)
{
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_trianglePipeline->getHandle());
    vkCmdDraw(cmd, 3, 1, 0, 0);
}

void TriangleFeature::onResize(uint32_t width, uint32_t height)
{
    m_drawExtent = {width, height};
}

RenderPassType TriangleFeature::getType() const
{
    return RenderPassType::Graphics;
}
