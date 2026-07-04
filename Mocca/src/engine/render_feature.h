#pragma once

#include <volk.h>

// TODO: children features should request pipeline instead of creating new one
// probably implement this when 2 renderFeatures utilize exactly the same pipelines

// render features is the system that allows for "plug & run"
class RenderFeature
{
public:
    virtual ~RenderFeature() = default;

    // each feature requires this
    virtual void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) = 0;

    // optional overrides
    virtual void onResize(uint32_t width, uint32_t height) {};
    virtual void onUpdate(float deltaTime) {}

    void setEnabled(bool state)
    {
        m_isEnabled = state;
    }
    bool isEnabled() const
    {
        return m_isEnabled;
    }

private:
    bool m_isEnabled = true;
};