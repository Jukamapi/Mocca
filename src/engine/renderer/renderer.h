#pragma once

#include "engine/renderer/render_feature.h"
#include "engine/vk/frame_manager.h"

#include <vector>
#include <memory>


class Context; // forward dec

class Renderer
{
public:

    // needs a reference to the context to create stuff
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void pushFeature(std::unique_ptr<RenderFeature> feature);

    void drawFrame();

private:
    //swapchain
    FrameManager m_frameManager;
    std::vector<std::unique_ptr<RenderFeature>> m_features;
};