#include "swapchain.h"

#include "core/types.h"
#include "platform/vulkan/vk_check.h"


#include <algorithm>


Swapchain::Swapchain(
    const SwapchainSupportDetails& details,
    const QueueFamilyIndices& indices,
    VkDevice device,
    VkSurfaceKHR surface,
    Extent frameBufferSize,
    VkSwapchainKHR oldSwapchain
)
    : m_logicalDevice(device)
{

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(details.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(details.presentModes);
    VkExtent2D extent = chooseSwapExtent(details.capabilities, frameBufferSize.width, frameBufferSize.height);

    uint32_t imageCount = details.capabilities.minImageCount + 1;

    if(details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
    {
        imageCount = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        // if i want to render to a different target for post processing i need to change this
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    };


    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if(indices.graphicsFamily != indices.presentFamily)
    {
        // TODO: maybe change to EXCLUSIVE, but needs additional logic, concurrent mode gives worse perfomance
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        // remember this stores pointer, so if moved to other function it becomes dangling after constructor is done
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = details.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;

    VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapchain));


    // images
    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapchain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapchain, &imageCount, m_swapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;

    try
    {
        createImageViews();
    }
    catch(...)
    {
        if(m_swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr);
        }
    }
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
{
    for(const auto& availableFormat : availableFormats)
    {
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
           availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
{
    for(const auto& availablePresentMode : availablePresentModes)
    {
        if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& capabilities, uint32_t windowWidth, uint32_t windowHeight
) const
{
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = {windowWidth, windowHeight};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
}

void Swapchain::createImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());

    for(size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_swapChainImageFormat,
            .components =
                {VK_COMPONENT_SWIZZLE_IDENTITY,
                 VK_COMPONENT_SWIZZLE_IDENTITY,
                 VK_COMPONENT_SWIZZLE_IDENTITY,
                 VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        VK_CHECK(vkCreateImageView(m_logicalDevice, &createInfo, nullptr, &m_swapChainImageViews[i]));
    }
}

void Swapchain::destroyImageViews()
{
    for(auto imageView : m_swapChainImageViews)
    {
        vkDestroyImageView(m_logicalDevice, imageView, nullptr);
    }
}

Swapchain::Swapchain(Swapchain&& other) noexcept
    : m_swapchain(other.m_swapchain), m_logicalDevice(other.m_logicalDevice),
      m_swapChainImageFormat(other.m_swapChainImageFormat), m_swapChainExtent(other.m_swapChainExtent),
      m_swapChainImages(std::move(other.m_swapChainImages)),
      m_swapChainImageViews(std::move(other.m_swapChainImageViews))
{
    other.m_swapchain = VK_NULL_HANDLE;
    other.m_logicalDevice = VK_NULL_HANDLE;
}

Swapchain& Swapchain::operator=(Swapchain&& other) noexcept
{
    if(this != &other)
    {
        destroyImageViews();

        if(m_swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr);
        }

        m_swapchain = other.m_swapchain;
        m_logicalDevice = other.m_logicalDevice;
        m_swapChainImageFormat = other.m_swapChainImageFormat;
        m_swapChainExtent = other.m_swapChainExtent;
        m_swapChainImages = std::move(other.m_swapChainImages);
        m_swapChainImageViews = std::move(other.m_swapChainImageViews);

        other.m_swapchain = VK_NULL_HANDLE;
        other.m_logicalDevice = VK_NULL_HANDLE;
    }

    return *this;
}

Swapchain::~Swapchain()
{
    destroyImageViews();

    if(m_swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr);
    }
}