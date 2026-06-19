#include "swapchain_manager.h"

#include "core/types.h"
#include "platform/vulkan/core/physical_device.h"
#include "platform/vulkan/resources/swapchain.h"

SwapchainManager::SwapchainManager(
    const PhysicalDevice& physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, Extent initialExtent
)
    : m_physicalDevice(physicalDevice), m_logicalDevice(logicalDevice), m_surface(surface)
{
    m_swapchain = std::make_unique<Swapchain>(
        m_physicalDevice.querySwapChainSupport(m_physicalDevice.getHandle(), m_surface),
        m_physicalDevice.getQueueFamilyIndices(),
        m_logicalDevice,
        m_surface,
        m_currentExtent
    );
}

void SwapchainManager::recreate(Extent newExtent)
{
    m_currentExtent = newExtent;

    vkDeviceWaitIdle(m_logicalDevice);

    SwapchainSupportDetails details = m_physicalDevice.querySwapChainSupport(m_physicalDevice.getHandle(), m_surface);

    Swapchain newSwapchain{
        details,
        m_physicalDevice.getQueueFamilyIndices(),
        m_logicalDevice,
        m_surface,
        newExtent,
        m_swapchain->getHandle()
    };

    *m_swapchain = std::move(newSwapchain);
}

ResizeResult SwapchainManager::handleResize(Extent newExtent)
{
    if(!m_isDirty)
        return ResizeResult::Ready;

    if(newExtent.width == 0 || newExtent.height == 0)
        return ResizeResult::Skipped;

    recreate(newExtent);
    m_isDirty = false;
    return ResizeResult::Recreated;
}