#pragma once

#include <volk.h>

class Context;
class Swapchain;
class DeletionQueue;

// TODO: in children feature should request pipeline instead of owning new one, will help me later


class RenderFeature
{
public:
    virtual ~RenderFeature() = default;

    virtual void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) = 0;

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