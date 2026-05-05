#pragma once

#include "Mocca/vulkan/vma_alloc.h"

#include <volk.h>

#include <cstdint>
#include <optional>
#include <vector>

enum class ResizeResult
{
    Ready,
    Skipped,
    Recreated
};

struct AllocatedImage
{
    VkImage image{VK_NULL_HANDLE};
    VkImageView imageView{VK_NULL_HANDLE};
    VmaAllocation allocation{VK_NULL_HANDLE};
    VkExtent3D imageExtent{};
    VkFormat imageFormat{VK_FORMAT_UNDEFINED};
};

struct Extent
{
    Extent() = default;
    Extent(VkExtent2D extent) noexcept : width(extent.width), height(extent.height) {}
    Extent(uint32_t w, uint32_t h) noexcept : width(w), height(h) {}

    uint32_t width, height;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};