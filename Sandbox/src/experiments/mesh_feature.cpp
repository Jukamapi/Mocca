#include "mesh_feature.h"

#include "renderer/draw_types.h"
#include "renderer/pipelines/graphics_pipeline.h"
#include "renderer/renderer.h"
#include "resource/asset_manager.h"
#include "resource/loader.h"
#include "scene/scene.h"


#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <numeric>

MeshFeature::MeshFeature(Renderer& renderer, AssetManager& assetManager, const Scene& scene)
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
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .enableDepthTest = true,
            .enableDepthWrite = false,
            .blendMode = BlendMode::Additive,
        }
    );
}

void MeshFeature::onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex)
{
    if(!m_scene)
        return;

    const DrawContext& drawContext = m_scene->getDrawContext();

    VkDescriptorSet globalSet = m_renderer.getGlobalUniforms().getDescriptorSet(frameIndex);

    renderObjects(cmd, m_opaquePipeline, globalSet, drawContext.opaqueSurfaces);

    renderObjects(cmd, m_transparentPipeline, globalSet, drawContext.transparentSurfaces);
}


void MeshFeature::onResize(uint32_t width, uint32_t height)
{
    m_drawExtent = {width, height};
}

RenderPassType MeshFeature::getType() const
{
    return RenderPassType::Graphics;
}


void MeshFeature::renderObjects(
    VkCommandBuffer cmd, GraphicsPipeline* pipeline, VkDescriptorSet globalSet, const std::vector<RenderObject>& objects
)
{
    bool isTransparent = (pipeline == m_transparentPipeline);

    m_sortIndices.resize(objects.size());
    std::iota(m_sortIndices.begin(), m_sortIndices.end(), 0);

    MaterialInstance* defaultMaterial = &m_assetManager.getDefaultMaterial();

    if(isTransparent)
    {
        glm::vec3 cameraPos = m_scene->getCamera().position;

        std::sort(
            m_sortIndices.begin(),
            m_sortIndices.end(),
            [&](uint32_t a, uint32_t b)
            {
                float distA = glm::length2(glm::vec3(objects[a].transform[3]) - cameraPos);
                float distB = glm::length2(glm::vec3(objects[b].transform[3]) - cameraPos);
                return distA > distB;
            }
        );
    }
    else
    {
        std::sort(
            m_sortIndices.begin(),
            m_sortIndices.end(),
            [&](uint32_t a, uint32_t b)
            {
                const auto& objA = objects[a];
                const auto& objB = objects[b];

                bool doubleSidedA = objA.material ? objA.material->doubleSided : false;
                bool doubleSidedB = objB.material ? objB.material->doubleSided : false;

                if(doubleSidedA != doubleSidedB)
                    return doubleSidedA < doubleSidedB;

                VkDescriptorSet setA = objA.material ? objA.material->materialSet : defaultMaterial->materialSet;
                VkDescriptorSet setB = objB.material ? objB.material->materialSet : defaultMaterial->materialSet;

                if(setA != setB)
                    return setA < setB;

                return objA.indexBuffer < objB.indexBuffer;
            }
        );
    }

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getLayout(), 0, 1, &globalSet, 0, nullptr);

    VkCullModeFlags currentCullMode = VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
    VkDescriptorSet currentMaterialSet = VK_NULL_HANDLE;
    VkBuffer currentIndexBuffer = VK_NULL_HANDLE;

    for(uint32_t idx : m_sortIndices)
    {
        const auto& obj = objects[idx];

        MaterialInstance* material = obj.material ? obj.material : &m_assetManager.getDefaultMaterial();

        VkCullModeFlags targetCull = material->doubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
        if(targetCull != currentCullMode)
        {
            currentCullMode = targetCull;
            vkCmdSetCullMode(cmd, currentCullMode);
        }

        if(material->materialSet != currentMaterialSet)
        {
            currentMaterialSet = material->materialSet;

            vkCmdBindDescriptorSets(
                cmd,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline->getLayout(),
                1,
                1,
                &currentMaterialSet,
                0,
                nullptr
            );
        }

        if(obj.indexBuffer != currentIndexBuffer)
        {
            currentIndexBuffer = obj.indexBuffer;

            vkCmdBindIndexBuffer(cmd, currentIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }

        GPUDrawPushConstants pushConstants{.worldMatrix = obj.transform, .vertexBuffer = obj.vertexBufferAddress};

        vkCmdPushConstants(
            cmd,
            pipeline->getLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(GPUDrawPushConstants),
            &pushConstants
        );

        // TODO MOCCA: implement vkCmdDrawIndexedIndirect
        vkCmdDrawIndexed(cmd, obj.indexCount, 1, obj.firstIndex, 0, 0);
    }
}
