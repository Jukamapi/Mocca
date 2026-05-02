#pragma once

#include <volk.h>

class Context;
class Swapchain;

class RenderFeature
{
public:
    virtual ~RenderFeature() = default;

    virtual void onAttach(VkDevice device, VkFormat colorFormat, VkExtent2D extent) = 0;
    virtual void onRender(VkCommandBuffer cmd) = 0;

    virtual void onDetach() {}
    virtual void onUpdate(float deltaTime) {}
    virtual void onResize(uint32_t width, uint32_t height) {}
};