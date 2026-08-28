#pragma once

#include <volk.h>
#include "core/types.h"

// allows for "plug and run"
class RenderFeature
{
public:
    virtual ~RenderFeature() = default;

    // each feature requires this
    virtual void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) = 0;

    // optional overrides
    virtual void onResize(uint32_t width, uint32_t height) {};
    virtual void onUpdate(float deltaTime) {}
    virtual void onImgui() {}

    void setEnabled(bool state)
    {
        m_isEnabled = state;
    }
    bool isEnabled() const
    {
        return m_isEnabled;
    }

    virtual RenderPassType getType() const = 0;

private:
    bool m_isEnabled = true;
};