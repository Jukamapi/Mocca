#pragma once

#include "render_feature.h"

#include <vector>
#include <memory>


class Context; // forward dec

class Renderer
{
public:

    // needs a reference to the context to create stuff
    Renderer(Context& context);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void pushFeature(std::unique_ptr<RenderFeature> feature);

    void drawFrame();

private:
    Context* m_context{ nullptr };
    std::vector<std::unique_ptr<RenderFeature>> m_features;
};