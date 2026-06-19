#include "surface.h"

#include <SDL_vulkan.h>
#include <stdexcept>


Surface::Surface(SDL_Window* window, VkInstance instance) : m_instance(instance)
{
    if(SDL_Vulkan_CreateSurface(window, instance, &m_surface) != SDL_TRUE)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

Surface::Surface(Surface&& other) noexcept : m_surface(other.m_surface), m_instance(other.m_instance)
{
    other.m_surface = VK_NULL_HANDLE;
    other.m_instance = VK_NULL_HANDLE;
}

Surface& Surface::operator=(Surface&& other) noexcept
{
    if(this != &other)
    {
        if(m_surface != VK_NULL_HANDLE)
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        m_surface = other.m_surface;
        m_instance = other.m_instance;

        other.m_surface = VK_NULL_HANDLE;
        other.m_instance = VK_NULL_HANDLE;
    }
    return *this;
}

Surface::~Surface()
{
    if(m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }
}