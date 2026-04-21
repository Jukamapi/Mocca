#pragma once

#include <vulkan/vulkan.h>

class Context;

class RenderFeature
{
public:
    virtual ~RenderFeature() = default;

    virtual void onAttach() = 0;
    virtual void onRender(VkCommandBuffer cmd) = 0;
    virtual void onDetach() = 0;

    virtual void onResize(uint32_t width, uint32_t height) {}
};