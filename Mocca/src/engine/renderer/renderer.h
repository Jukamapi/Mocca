#pragma once

#include "core/types.h"
#include "engine/renderer/render_feature.h"
#include "platform/vulkan/commands/frame_manager.h"
#include "platform/vulkan/resources/swapchain_manager.h"


#include <functional>
#include <memory>
#include <vector>


class Swapchain;
class Context;

class Renderer
{
public:
    using ExtentProvider = std::function<Extent()>;

    Renderer(const Context& context, ExtentProvider extentProvider);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void pushFeature(std::unique_ptr<RenderFeature> feature);
    void drawFrame();

    const std::vector<std::unique_ptr<RenderFeature>>& getFeatures() const
    {
        return m_features;
    }

    template <typename T> T* getFeature()
    {
        for(auto& feature : m_features)
        {
            T* casted = dynamic_cast<T*>(feature.get());
            if(casted)
                return casted;
        }
        return nullptr;
    }

    void markSwapchainDirty()
    {
        m_swapchainManager.markDirty();
    }

private:
    const Context& m_context;
    ExtentProvider m_extentProvider;
    SwapchainManager m_swapchainManager;
    FrameManager m_frameManager;
    bool m_isSuspended{false};

    std::vector<std::unique_ptr<RenderFeature>> m_features;

    bool acquireNextImage(uint32_t& outImageIndex);

    VkCommandBuffer recordCommandBuffer(uint32_t imageIndex);
    void submitAndPresent(uint32_t imageIndex, VkCommandBuffer cmd);

    bool processResize();
    void recreateSwapchain(Extent newExtent);

    void transitionImage(
        VkCommandBuffer cmd,
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkAccessFlags2 srcAccess,
        VkAccessFlags2 dstAccess,
        VkPipelineStageFlags2 srcStage,
        VkPipelineStageFlags2 dstStage
    );
};