#pragma once

#include "engine/render_feature.h"
#include "engine/renderer.h"
#include "platform/vulkan/pipeline/graphics_pipeline.h"
#include "platform/vulkan/pipeline/pipeline_manager.h"
#include "resource/loader.h"

#include <imgui.h>


class MeshFeature : public RenderFeature
{
public:
    MeshFeature(Renderer& renderer)
        : m_device(renderer.getContext().getLogicalDevice().getHandle()),
          m_drawExtent(renderer.getExtent()),
          m_rectangle(&renderer.getRectangleMesh())
    {

        auto vertShader = loadShader("colored_triangle_mesh.vert.spv");
        auto fragShader = loadShader("colored_triangle.frag.spv");

        auto& pipelineManager = renderer.getPipelineManager();

        VkPushConstantRange bufferRange{
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(GPUDrawPushConstants),
        };

        m_meshPipeline = &pipelineManager.createGraphicsPipeline(
            "mesh",
            {
                .colorFormat = renderer.getDrawFormat(),
                .depthFormat = renderer.getDepthFormat(),
                .vertCode = vertShader,
                .fragCode = fragShader,
                .pushConstants = {bufferRange},
            }
        );
    }

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override
    {

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_meshPipeline->getHandle());

        GPUDrawPushConstants pushConstants{
            .worldMatrix = glm::mat4{1.f},
            .vertexBuffer = m_rectangle->vertexBufferAddress
        };

        vkCmdPushConstants(
            cmd,
            m_meshPipeline->getLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(GPUDrawPushConstants),
            &pushConstants
        );

        vkCmdBindIndexBuffer(cmd, m_rectangle->indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
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

    GraphicsPipeline* m_meshPipeline;

    VkExtent2D m_drawExtent{};

    const GPUMeshBuffers* m_rectangle{nullptr};
};