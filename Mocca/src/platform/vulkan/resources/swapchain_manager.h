#pragma once

#include "core/types.h"

#include <functional>
#include <memory>

struct Extent;
class Context;
class Swapchain;
enum class ResizeResult;

class SwapchainManager
{
public:
    using ExtentProvider = std::function<Extent()>;

    SwapchainManager(const Context& context, Extent initialExtent);

    ResizeResult handleResize(Extent newExtent);
    void recreate(Extent newExtent);

    Swapchain& getSwapchain()
    {
        return *m_swapchain;
    }

    void markDirty()
    {
        m_isDirty = true;
    }

    bool isDirty() const
    {
        return m_isDirty;
    }


private:
    const Context& m_context;
    Extent m_currentExtent;
    std::unique_ptr<Swapchain> m_swapchain;
    bool m_isDirty{false};
};