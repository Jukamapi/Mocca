#pragma once

#include <volk.h>

class Context;
class Swapchain;
class DeletionQueue;

// TODO: in children feature should request pipeline instead of owning new one, will help me later

// TODO: Asset manager will also help here with onResize so i dont have to loadShader() every time
class RenderFeature
{
public:
    virtual ~RenderFeature() = default;

    virtual void onAttach(VkDevice device, VkFormat colorFormat, VkExtent2D extent) = 0;
    virtual void onRender(VkCommandBuffer cmd) = 0;

    // pass VkFormat colorFormat, DeletionQueue& dq?
    virtual void onResize(uint32_t width, uint32_t height) {};
    virtual void onDetach() {}
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