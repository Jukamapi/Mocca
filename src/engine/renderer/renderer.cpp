#include "renderer.h"

Renderer::Renderer()
    : m_frameManager()
{
}


void Renderer::drawFrame()
{
    //  wait for fence
    //  currentFrame.deletionQueue.flush();
    //  reset fence

    //  start rendering
    //  execute all features in order
    // for (auto& feature : m_features)
}

void Renderer::pushFeature(std::unique_ptr<RenderFeature> feature)
{
    feature->onAttach();
    m_features.push_back(std::move(feature));
}

Renderer::~Renderer()
{
    //  detach features
    //
}