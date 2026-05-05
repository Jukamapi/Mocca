#pragma once

#include <volk.h>
#include <vector>

struct SwapchainSupportDetails;
struct QueueFamilyIndices;


class PhysicalDevice;
struct Extent;

class Swapchain
{
public:
    Swapchain(
        const SwapchainSupportDetails& details,
        const QueueFamilyIndices& indices,
        VkDevice device,
        VkSurfaceKHR surface,
        Extent frameBufferSize,
        VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE
    );
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    Swapchain(Swapchain&&) noexcept;
    Swapchain& operator=(Swapchain&&) noexcept;

    VkExtent2D getExtent() const
    {
        return m_swapChainExtent;
    }
    VkFormat getFormat() const
    {
        return m_swapChainImageFormat;
    }
    VkSwapchainKHR getHandle() const
    {
        return m_swapchain;
    }
    std::vector<VkImage> getImages() const
    {
        return m_swapChainImages;
    }
    std::vector<VkImageView> getImageViews() const
    {
        return m_swapChainImageViews;
    }

private:
    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};
    VkDevice m_logicalDevice{VK_NULL_HANDLE};
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;

    // helpers
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D chooseSwapExtent(
        const VkSurfaceCapabilitiesKHR& capabilities, uint32_t windowWidth, uint32_t windowHeight
    ) const;

    void createImageViews();
    void destroyImageViews();
};