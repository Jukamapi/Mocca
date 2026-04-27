#pragma once

#include "engine/vk/instance.h"
#include "engine/vk/surface.h"
#include "engine/vk/physical_device.h"
#include "engine/vk/logical_device.h"

#include <volk.h>

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
    Surface m_surface;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;


    // todo: instance, device
    //VkDevice m_device{};
};