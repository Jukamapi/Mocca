#pragma once

#include "renderer/draw_types.h"
#include "renderer/pipelines/graphics_pipeline.h"
#include "renderer/pipelines/pipeline_manager.h"
#include "renderer/render_feature.h"
#include "renderer/renderer.h"
#include "resource/asset_manager.h"
#include "resource/loader.h"


#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


class MeshFeature : public RenderFeature
{
public:
    MeshFeature(Renderer& renderer, AssetManager& assetManager, const std::vector<std::shared_ptr<MeshAsset>>* meshes)
        : m_renderer(renderer),
          m_assetManager(assetManager),
          m_testMeshes(meshes),
          m_drawExtent(renderer.getExtent())
    {
        auto vertShader = loadShader("mesh.vert.spv");
        auto fragShader = loadShader("mesh.frag.spv");

        auto& pipelineManager = renderer.getPipelineManager();

        VkPushConstantRange bufferRange{
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(GPUDrawPushConstants),
        };

        std::vector<VkDescriptorSetLayout> setLayouts = {
            renderer.getGlobalUniforms().getLayout(),
            renderer.getMaterialLayout().getHandle()
        };

        m_opaquePipeline = &pipelineManager.createGraphicsPipeline(
            "gltf_opaque",
            {
                .colorFormat = renderer.getDrawFormat(),
                .depthFormat = renderer.getDepthFormat(),
                .vertCode = vertShader,
                .fragCode = fragShader,
                .descriptorLayouts = setLayouts,
                .pushConstants = {bufferRange},
                .cullMode = VK_CULL_MODE_NONE,
                .frontFace = VK_FRONT_FACE_CLOCKWISE,
                .enableDepthTest = true,
                .enableDepthWrite = true,
                .blendMode = BlendMode::None,
            }
        );

        m_transparentPipeline = &pipelineManager.createGraphicsPipeline(
            "gltf_transparent",
            {
                .colorFormat = renderer.getDrawFormat(),
                .depthFormat = renderer.getDepthFormat(),
                .vertCode = vertShader,
                .fragCode = fragShader,
                .descriptorLayouts = setLayouts,
                .pushConstants = {bufferRange},
                .cullMode = VK_CULL_MODE_NONE,
                .frontFace = VK_FRONT_FACE_CLOCKWISE,
                .enableDepthTest = true,
                .enableDepthWrite = false,
                .blendMode = BlendMode::Additive,
            }
        );
    }

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override
    {

        if(!m_testMeshes || m_testMeshes->empty())
            return;

        glm::mat4 view = glm::translate(glm::vec3{0, 0, -5});

        glm::mat4 projection =
            glm::perspective(glm::radians(70.f), (float)m_drawExtent.width / (float)m_drawExtent.height, 10000.f, 0.1f);

        projection[1][1] *= -1;

        m_renderer.getGlobalUniforms().update(
            frameIndex,
            {.view = view,
             .proj = projection,
             .viewproj = projection * view,
             .ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
             .sunlightDirection = glm::normalize(glm::vec4(0.5f, 1.0f, 0.5f, 1.0f)),
             .sunlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.5f)}
        );

        VkDescriptorSet globalSet = m_renderer.getGlobalUniforms().getDescriptorSet(frameIndex);

        GraphicsPipeline* currentPipeline = nullptr;
        VkBuffer currentIndexBuffer = VK_NULL_HANDLE;

        for(const auto& mesh : *m_testMeshes)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);

            for(const auto& surface : mesh->surfaces)
            {
                MaterialInstance* material =
                    surface.material ? surface.material.get() : &m_assetManager.getDefaultMaterial();

                GraphicsPipeline* targetPipeline =
                    (material->passType == MaterialPass::Transparent) ? m_transparentPipeline : m_opaquePipeline;

                if(targetPipeline != currentPipeline)
                {
                    currentPipeline = targetPipeline;
                    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->getHandle());

                    vkCmdBindDescriptorSets(
                        cmd,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        currentPipeline->getLayout(),
                        0,
                        1,
                        &globalSet,
                        0,
                        nullptr
                    );
                }

                vkCmdBindDescriptorSets(
                    cmd,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    currentPipeline->getLayout(),
                    1,
                    1,
                    &material->materialSet,
                    0,
                    nullptr
                );


                if(mesh->meshBuffers.indexBuffer.getBuffer() != currentIndexBuffer)
                {
                    currentIndexBuffer = mesh->meshBuffers.indexBuffer.getBuffer();
                    vkCmdBindIndexBuffer(cmd, currentIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
                }


                GPUDrawPushConstants pushConstants{
                    .worldMatrix = modelMatrix,
                    .vertexBuffer = mesh->meshBuffers.vertexBufferAddress,
                };

                vkCmdPushConstants(
                    cmd,
                    currentPipeline->getLayout(),
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof(GPUDrawPushConstants),
                    &pushConstants
                );

                vkCmdDrawIndexed(cmd, surface.count, 1, surface.startIndex, 0, 0);
            }
        }
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
    Renderer& m_renderer;
    AssetManager& m_assetManager;
    const std::vector<std::shared_ptr<MeshAsset>>* m_testMeshes = nullptr;
    VkExtent2D m_drawExtent{};

    GraphicsPipeline* m_opaquePipeline{nullptr};
    GraphicsPipeline* m_transparentPipeline{nullptr};
};