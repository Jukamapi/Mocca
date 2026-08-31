#include "swapchain_manager.h"

#include "core/types.h"
#include "platform/vulkan/context/physical_device.h"
#include "swapchain.h"

SwapchainManager::SwapchainManager(
    const PhysicalDevice& physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, Extent initialExtent
)
    : m_physicalDevice(physicalDevice),
      m_logicalDevice(logicalDevice),
      m_surface(surface),
      m_currentExtent(initialExtent)
{
    m_swapchain = std::make_unique<Swapchain>(
        m_physicalDevice.querySwapChainSupport(m_physicalDevice.getHandle(), m_surface),
        m_physicalDevice.getQueueFamilyIndices(),
        m_logicalDevice,
        m_surface,
        m_currentExtent
    );
}

bool SwapchainManager::recreate(Extent newExtent)
{

    SwapchainSupportDetails details = m_physicalDevice.querySwapChainSupport(m_physicalDevice.getHandle(), m_surface);

    VkExtent2D resolvedExtent = Swapchain::chooseSwapExtent(details.capabilities, newExtent.width, newExtent.height);

    if(resolvedExtent.width == 0 || resolvedExtent.height == 0)
        return false;

    m_currentExtent = {resolvedExtent.width, resolvedExtent.height};

    vkDeviceWaitIdle(m_logicalDevice);

    Swapchain newSwapchain{
        details,
        m_physicalDevice.getQueueFamilyIndices(),
        m_logicalDevice,
        m_surface,
        m_currentExtent,
        m_swapchain->getHandle()
    };

    *m_swapchain = std::move(newSwapchain);
    return true;
}

ResizeResult SwapchainManager::handleResize(Extent newExtent)
{
    if(!m_isDirty)
        return ResizeResult::Ready;

    if(newExtent.width == 0 || newExtent.height == 0)
        return ResizeResult::Skipped;

    if(recreate(newExtent))
    {
        m_isDirty = false;
        return ResizeResult::Recreated;
    }

    return ResizeResult::Skipped;
}