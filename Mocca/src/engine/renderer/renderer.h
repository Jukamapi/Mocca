#pragma once

#include "core/types.h"
#include "engine/renderer/render_feature.h"
#include "platform/vulkan/commands/frame_manager.h"
#include "platform/vulkan/core/context.h"
#include "platform/vulkan/resources/swapchain_manager.h"


#include <functional>
#include <memory>
#include <vector>


class Swapchain;

class Renderer
{
public:
    using ExtentProvider = std::function<Extent()>;

    Renderer(const Window& window, ExtentProvider extentProvider);
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

    const Context& getContext() const
    {
        return m_context;
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
    Context m_context;
    ExtentProvider m_extentProvider;
    VkExtent2D m_renderExtent;
    SwapchainManager m_swapchainManager;
    FrameManager m_frameManager;
    std::vector<std::unique_ptr<RenderFeature>> m_features;

    bool m_isSuspended{false};

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
        VkPipelineStageFlags2 dstStage,
        VkImageAspectFlags aspectMask
    );

    void blitImage(VkCommandBuffer cmd, VkImage src, VkExtent2D srcExtent, VkImage dst, VkExtent2D dstExtent);

    void createFrameImages();
    void destroyFrameImages();

    static constexpr VkFormat DRAW_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
    static constexpr VkFormat DEPTH_FORMAT{VK_FORMAT_D32_SFLOAT};
};