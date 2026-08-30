#pragma once

#include "gpu_alloc.h"
#include "instance.h"
#include "logical_device.h"
#include "physical_device.h"
#include "surface.h"

#include <volk.h>

class Window;

// class holding static lifetime data required by vulkan
class Context
{
public:
    Context(const Window& window);
    ~Context() = default;
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;

    const Instance& getInstance() const
    {
        return m_instance;
    }
    const Surface& getSurface() const
    {
        return m_surface;
    }
    const PhysicalDevice& getPhysicalDevice() const
    {
        return m_physicalDevice;
    }
    const LogicalDevice& getLogicalDevice() const
    {
        return m_logicalDevice;
    }
    const GpuAlloc& getVmaAlloc() const
    {
        return m_vmaAlloc;
    }

private:
    Instance m_instance;
    Surface m_surface;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
    GpuAlloc m_vmaAlloc;
};