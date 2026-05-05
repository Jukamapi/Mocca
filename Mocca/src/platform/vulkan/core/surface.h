#pragma once

#include <volk.h>

struct SDL_Window;

class Surface
{
public:
    Surface(SDL_Window* window, VkInstance instance);
    ~Surface();
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    Surface(Surface&&) = delete;
    Surface& operator=(Surface&&) = delete;

    VkSurfaceKHR getHandle() const
    {
        return m_surface;
    }

private:
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkInstance m_instance{VK_NULL_HANDLE};
};