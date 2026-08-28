#pragma once

#include <cstdint>

// helper class for Swapchain recreation
enum class ResizeResult
{
    Ready,
    Skipped,
    Recreated
};

// this allows me to not have to include volk everywhere, might be overkill
struct Extent
{
    Extent() = default;
    Extent(uint32_t w, uint32_t h) noexcept
        : width(w),
          height(h)
    {
    }

    uint32_t width, height;
};

enum class RenderPassType
{
    Compute,
    Graphics
};