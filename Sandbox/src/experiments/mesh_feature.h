#pragma once

#include "core/vk_types.h"
#include "renderer/pipelines/graphics_pipeline.h"
#include "renderer/pipelines/pipeline_manager.h"
#include "renderer/render_feature.h"
#include "renderer/renderer.h"
#include "resource/loader.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


class MeshFeature : public RenderFeature
{
public:
    MeshFeature(Renderer& renderer, const std::vector<std::shared_ptr<MeshAsset>>* meshes)
        : m_device(renderer.getContext().getLogicalDevice().getHandle()),
          m_drawExtent(renderer.getExtent()),
          m_testMeshes(meshes)
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

        glm::mat4 view = glm::translate(glm::vec3{0, 0, -5});

        glm::mat4 projection =
            glm::perspective(glm::radians(70.f), (float)m_drawExtent.width / (float)m_drawExtent.height, 10000.f, 0.1f);

        projection[1][1] *= -1;

        GPUDrawPushConstants pushConstants{
            .worldMatrix = projection * view,
            .vertexBuffer = m_testMeshes->at(2)->meshBuffers.vertexBufferAddress,
        };

        vkCmdPushConstants(
            cmd,
            m_meshPipeline->getLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(GPUDrawPushConstants),
            &pushConstants
        );

        vkCmdBindIndexBuffer(cmd, m_testMeshes->at(2)->meshBuffers.indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(
            cmd,
            m_testMeshes->at(2)->surfaces[0].count,
            1,
            m_testMeshes->at(2)->surfaces[0].startIndex,
            0,
            0
        );

        // if(!m_testMeshes)
        //     return;

        // for(const auto& mesh : *m_testMeshes)
        // {
        //     // here draw calls
        // }
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

    const std::vector<std::shared_ptr<MeshAsset>>* m_testMeshes = nullptr;
};