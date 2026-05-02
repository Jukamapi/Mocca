#pragma once

#include "Mocca/core/types.h"
#include "Mocca/renderer/render_feature.h"
#include "Mocca/vulkan/commands/frame_manager.h"

#include <functional>
#include <memory>
#include <vector>


class Swapchain;
class Context; // forward dec

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

    void markSwapchainDirty()
    {
        m_isSwapchainDirty = true;
    }

private:
    const Context& m_context;
    ExtentProvider m_extentProvider;
    FrameManager m_frameManager;
    Extent m_currentExtent;
    bool m_isSwapchainDirty{false};
    bool m_isSuspended{false};

    std::vector<std::unique_ptr<RenderFeature>> m_features;
    std::unique_ptr<Swapchain> m_swapchain;

    void recreateSwapchain(Extent newExtent);
};