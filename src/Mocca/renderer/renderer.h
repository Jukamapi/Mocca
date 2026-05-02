#pragma once

#include "Mocca/renderer/render_feature.h"
#include "Mocca/vulkan/commands/frame_manager.h"


#include <memory>
#include <vector>

class Swapchain;
class Context; // forward dec
struct Extent;

class Renderer
{
public:
    Renderer(const Context& context, Extent drawableSize);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void pushFeature(std::unique_ptr<RenderFeature> feature);
    void recreateSwapchain();

    void drawFrame();

private:
    const Context& m_context;
    FrameManager m_frameManager;
    std::vector<std::unique_ptr<RenderFeature>> m_features;
    std::unique_ptr<Swapchain> m_swapchain;
};