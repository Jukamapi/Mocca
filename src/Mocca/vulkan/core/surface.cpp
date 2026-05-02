#include "surface.h"


#include <volk.h>
#include <SDL_vulkan.h>
#include <stdexcept>


Surface::Surface(SDL_Window* window, VkInstance instance) : m_instance(instance)
{
    if(SDL_Vulkan_CreateSurface(window, instance, &m_surface) != SDL_TRUE)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

Surface::~Surface()
{
    if(m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }
}