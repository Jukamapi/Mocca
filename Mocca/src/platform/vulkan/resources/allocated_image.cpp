#include "allocated_image.h"

#include "platform/vulkan/vk_check.h"

AllocatedImage::AllocatedImage(
    VkDevice device,
    VmaAllocator allocator,
    VkExtent3D extent,
    VkFormat format,
    VkImageUsageFlags usage,
    VkImageAspectFlags aspect
)
    : m_extent(extent), m_format(format), m_device(device), m_allocator(allocator)
{
    VkImageCreateInfo imageInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = m_format,
        .extent = m_extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
    };

    VmaAllocationCreateInfo allocInfo{
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    VK_CHECK(vmaCreateImage(m_allocator, &imageInfo, &allocInfo, &m_image, &m_allocation, nullptr));

    try
    {
        createImageView(aspect);
    }
    catch(...)
    {
        vmaDestroyImage(m_allocator, m_image, m_allocation);
        throw;
    }
}

void AllocatedImage::createImageView(VkImageAspectFlags aspect)
{
    VkImageViewCreateInfo viewInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = m_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = m_format,
        .subresourceRange = {
            .aspectMask = aspect,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    VK_CHECK(vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageView));
}


void AllocatedImage::destroy()
{
    if(m_imageView != VK_NULL_HANDLE)
        vkDestroyImageView(m_device, m_imageView, nullptr);

    if(m_image != VK_NULL_HANDLE)
        vmaDestroyImage(m_allocator, m_image, m_allocation);

    m_image = VK_NULL_HANDLE;
    m_imageView = VK_NULL_HANDLE;
    m_allocation = nullptr;
}

AllocatedImage::AllocatedImage(AllocatedImage&& other) noexcept
    : m_image(other.m_image), m_imageView(other.m_imageView), m_allocation(other.m_allocation),
      m_extent(other.m_extent), m_format(other.m_format), m_device(other.m_device), m_allocator(other.m_allocator)
{
    other.m_image = VK_NULL_HANDLE;
    other.m_imageView = VK_NULL_HANDLE;
    other.m_allocation = nullptr;
}

AllocatedImage& AllocatedImage::operator=(AllocatedImage&& other) noexcept
{
    if(this != &other)
    {
        destroy();

        m_image = other.m_image;
        m_imageView = other.m_imageView;
        m_allocation = other.m_allocation;
        m_extent = other.m_extent;
        m_format = other.m_format;
        m_device = other.m_device;
        m_allocator = other.m_allocator;

        other.m_image = VK_NULL_HANDLE;
        other.m_imageView = VK_NULL_HANDLE;
        other.m_allocation = nullptr;
    }
    return *this;
}

AllocatedImage::~AllocatedImage()
{
    destroy();
}