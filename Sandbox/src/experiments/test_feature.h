#pragma once

#include "engine/renderer/render_feature.h"
#include "platform/vulkan/pipeline/pipeline.h"
#include "resource/loader.h"


#include <memory>


// TODO: it uses too much layers: loadShader -> resource (replace by AssetManager), directly touches m_pipeline and
// vulkan
class TestFeature : public RenderFeature
{
public:
    void onAttach(VkDevice device, VkFormat colorFormat, VkExtent2D extent) override
    {
        auto vertCode = loadShader("shader.vert.spv");
        auto fragCode = loadShader("shader.frag.spv");

        m_pipeline = std::make_unique<Pipeline>(device, colorFormat, extent, vertCode, fragCode);
    }
    void onRender(VkCommandBuffer cmd) override
    {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getHandle());

        vkCmdDraw(cmd, 3, 1, 0, 0);
    }


private:
    std::unique_ptr<Pipeline> m_pipeline;
};