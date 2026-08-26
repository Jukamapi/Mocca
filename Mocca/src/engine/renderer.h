#pragma once

#include "core/types.h"
#include "engine/imgui_manager.h"
#include "engine/render_feature.h"
#include "platform/vulkan/commands/frame_manager.h"
#include "platform/vulkan/core/context.h"
#include "platform/vulkan/resources/swapchain_manager.h"

#include <functional>
#include <memory>
#include <vector>

class Swapchain;

// class handling main rendering logic
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

    void beginUiFrame()
    {
        m_imGuiManager.beginFrame();
    }

    void endUiFrame()
    {
        m_imGuiManager.endFrame();
    }

    const std::vector<std::unique_ptr<RenderFeature>>& getFeatures() const
    {
        return m_features;
    }


    const Context& getContext() const
    {
        return m_context;
    }

    // tries to cast RenderFeature into specific derived type
    template <typename T> T* getFeature()
    {
        for(auto& feature : m_features)
        {
            // tries to cast into type T, if not its nullptr
            T* casted = dynamic_cast<T*>(feature.get());
            if(casted)
                return casted;
        }
        return nullptr;
    }

    // informs swapchain that it needs to be recreated
    void markSwapchainDirty()
    {
        m_swapchainManager.markDirty();
    }

    const VkExtent2D getExtent() const
    {
        return {m_extentProvider().width, m_extentProvider().height};
    }

    const ExtentProvider& getExtentProvider() const
    {
        return m_extentProvider;
    }

private:
    bool acquireNextImage(uint32_t& outImageIndex);
    VkCommandBuffer recordCommandBuffer(uint32_t imageIndex);
    void submitAndPresent(uint32_t imageIndex, VkCommandBuffer cmd);

    // swapchain handling
    bool processResize();

    // helper method for changing images
    void transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    // copies an image from one place to another
    // handles scaling, format conversion between high precision to standard
    void blitImage(VkCommandBuffer cmd, VkImage src, VkExtent2D srcExtent, VkImage dst, VkExtent2D dstExtent);

    // allocates and deallocates color and depth images
    void createFrameImages();
    void destroyFrameImages();

    // constants
    static constexpr VkFormat DRAW_FORMAT{VK_FORMAT_R16G16B16A16_SFLOAT};
    static constexpr VkFormat DEPTH_FORMAT{VK_FORMAT_D32_SFLOAT};

    Context m_context;
    ExtentProvider m_extentProvider;
    VkExtent2D m_renderExtent;
    SwapchainManager m_swapchainManager;
    FrameManager m_frameManager;
    std::vector<std::unique_ptr<RenderFeature>> m_features;
    ImGuiManager m_imGuiManager;
    bool m_isSuspended{false};
};