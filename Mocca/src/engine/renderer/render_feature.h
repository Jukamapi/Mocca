#pragma once

#include <volk.h>

// TODO: in children feature should request pipeline instead of owning new one, will help later

// render features is the modular system this app uses that allows for "plug & run" methods
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