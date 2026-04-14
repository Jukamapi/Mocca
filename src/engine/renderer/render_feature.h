#pragma once

#include <vulkan/vulkan.h>

class Context;

class RenderFeature
{
public:
    virtual ~RenderFeature() = default;

    virtual void onAttach(Context& context) = 0;
    virtual void onRender(Context& context, VkCommandBuffer cmd) = 0;
    virtual void onDetach(Context& context) = 0;

    virtual void onResize(Context& context, uint32_t width, uint32_t height) {}
};