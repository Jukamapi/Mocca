#pragma once

#include "renderer/draw_types.h"
#include "renderer/pipelines/graphics_pipeline.h"
#include "renderer/render_feature.h"


class AssetManager;
class Scene;
class Renderer;

class MeshFeature : public RenderFeature
{
public:
    MeshFeature(Renderer& renderer, AssetManager& assetManager, const Scene& scene);


    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override;


    void onResize(uint32_t width, uint32_t height) override;

    RenderPassType getType() const override;

private:
    void renderObjects(
        VkCommandBuffer cmd,
        GraphicsPipeline* pipeline,
        VkDescriptorSet globalSet,
        const std::vector<RenderObject>& objects
    );

    Renderer& m_renderer;
    AssetManager& m_assetManager;

    // saving it as pointer so i can change scenes
    const Scene* m_scene{nullptr};
    VkExtent2D m_drawExtent{};

    GraphicsPipeline* m_opaquePipeline{nullptr};
    GraphicsPipeline* m_transparentPipeline{nullptr};

    std::vector<uint32_t> m_sortIndices;
};