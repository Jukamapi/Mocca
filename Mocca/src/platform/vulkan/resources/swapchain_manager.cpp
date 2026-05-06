#include "swapchain_manager.h"

#include "core/types.h"
#include "platform/vulkan/core/context.h"
#include "platform/vulkan/resources/swapchain.h"


SwapchainManager::SwapchainManager(const Context& context, Extent initialExtent)
    : m_context(context), m_currentExtent(initialExtent)
{
    m_swapchain = std::make_unique<Swapchain>(
        context.getPhysicalDevice()
            .querySwapChainSupport(context.getPhysicalDeviceHandle(), context.getSurfaceHandle()),
        context.getPhysicalDevice().getQueueFamilyIndices(),
        context.getDeviceHandle(),
        context.getSurfaceHandle(),
        m_currentExtent
    );
}

void SwapchainManager::recreate(Extent newExtent)
{
    m_currentExtent = newExtent;

    vkDeviceWaitIdle(m_context.getDeviceHandle());

    SwapchainSupportDetails details = m_context.getPhysicalDevice().querySwapChainSupport(
        m_context.getPhysicalDeviceHandle(),
        m_context.getSurfaceHandle()
    );

    Swapchain newSwapchain{
        details,
        m_context.getPhysicalDevice().getQueueFamilyIndices(),
        m_context.getDeviceHandle(),
        m_context.getSurfaceHandle(),
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