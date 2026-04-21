#pragma once

#include "engine/vk/instance.h"
#include <vulkan/vulkan.h>

#include <vulkan/vulkan_core.h>

class Window;

class Context
{
public:
    // .getNativeWindow() to create the surface
    Context(Window& window);
    ~Context();
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;


    VkInstance getInstance() const { return m_instance.getInstanceHandle(); }

    // might modiffy


private:
    Instance m_instance;

    // todo: instance, device
    //VkDevice m_device{};
};