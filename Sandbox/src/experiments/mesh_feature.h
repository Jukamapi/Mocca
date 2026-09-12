#pragma once

#include "renderer/draw_types.h"
#include "renderer/pipelines/graphics_pipeline.h"
#include "renderer/pipelines/pipeline_manager.h"
#include "renderer/render_feature.h"
#include "renderer/renderer.h"
#include "resource/asset_manager.h"
#include "resource/loader.h"
#include "scene/scene.h"


#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


class MeshFeature : public RenderFeature
{
public:
    MeshFeature(Renderer& renderer, AssetManager& assetManager, const Scene& scene)
        : m_renderer(renderer),
          m_assetManager(assetManager),
          m_scene(&scene),
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
        if(!m_scene)
            return;

        const DrawContext& drawContext = m_scene->getDrawContext();

        if(drawContext.opaqueSurfaces.empty())
            return;

        // camera
        float aspect = (float)m_drawExtent.width / (float)m_drawExtent.height;

        const Camera& camera = m_scene->getCamera();

        glm::mat4 view = camera.getViewMatrix();

        glm::mat4 projection = camera.getProjectionMatrix(aspect);

        m_renderer.getGlobalUniforms().update(
            frameIndex,
            {.view = view,
             .proj = projection,
             .viewproj = projection * view,
             .ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
             .sunlightDirection = glm::normalize(glm::vec4(0.5f, 1.0f, 0.5f, 1.0f)),
             .sunlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.5f)}
        );
        // end of camera

        VkDescriptorSet globalSet = m_renderer.getGlobalUniforms().getDescriptorSet(frameIndex);

        GraphicsPipeline* currentPipeline = nullptr;
        VkBuffer currentIndexBuffer = VK_NULL_HANDLE;

        for(const auto& obj : drawContext.opaqueSurfaces)
        {
            MaterialInstance* material = obj.material ? obj.material : &m_assetManager.getDefaultMaterial();

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

            if(obj.indexBuffer != currentIndexBuffer)
            {
                currentIndexBuffer = obj.indexBuffer;
                vkCmdBindIndexBuffer(cmd, currentIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
            }

            GPUDrawPushConstants pushConstants{.worldMatrix = obj.transform, .vertexBuffer = obj.vertexBufferAddress};

            vkCmdPushConstants(
                cmd,
                currentPipeline->getLayout(),
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(GPUDrawPushConstants),
                &pushConstants
            );

            vkCmdDrawIndexed(cmd, obj.indexCount, 1, obj.firstIndex, 0, 0);
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

    // saving it as pointer so i can change scenes
    const Scene* m_scene{nullptr};
    VkExtent2D m_drawExtent{};

    GraphicsPipeline* m_opaquePipeline{nullptr};
    GraphicsPipeline* m_transparentPipeline{nullptr};
};