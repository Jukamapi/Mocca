#pragma once

#include <cstdint>

enum class ResizeResult
{
    Ready,
    Skipped,
    Recreated
};

struct Extent
{
    Extent() = default;
    Extent(uint32_t w, uint32_t h) noexcept : width(w), height(h) {}

    uint32_t width, height;
};