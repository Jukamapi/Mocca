#pragma once

#include <volk.h>

#include "platform/vulkan/core/gpu_alloc.h"

class AllocatedImage
{
public:
    AllocatedImage() = default;
    AllocatedImage(
        VkDevice device,
        VmaAllocator allocator,
        VkExtent3D extent,
        VkFormat format,
        VkImageUsageFlags usage,
        VkImageAspectFlags aspect
    );
    ~AllocatedImage();
    AllocatedImage(const AllocatedImage&) = delete;
    AllocatedImage& operator=(const AllocatedImage&) = delete;
    AllocatedImage(AllocatedImage&&) noexcept;
    AllocatedImage& operator=(AllocatedImage&&) noexcept;

    void destroy();

    VkImage getImage() const
    {
        return m_image;
    }
    VkImageView getImageView() const
    {
        return m_imageView;
    }
    VkFormat getFormat() const
    {
        return m_format;
    }
    VkExtent3D getExtent() const
    {
        return m_extent;
    }

private:
    VkImage m_image{VK_NULL_HANDLE};
    VkImageView m_imageView{VK_NULL_HANDLE};
    VmaAllocation m_allocation{nullptr};
    VkExtent3D m_extent{};
    VkFormat m_format{};

    VkDevice m_device{VK_NULL_HANDLE};
    VmaAllocator m_allocator{nullptr};

    void createImageView(VkImageAspectFlags aspect);
};