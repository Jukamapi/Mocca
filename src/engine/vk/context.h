//vkinstance, vkphysicaldevice, vkdevice, vkqueue, maybe debug messenger
#pragma once

class Window;

class Context
{
public:
    // SDL_Window to create the surface
    Context(Window& window);
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    // VkInstance       instance;
    // VkPhysicalDevice physicalDevice;
    // VkDevice         device;
    // VkSurfaceKHR     surface;
    // VkQueue          graphicsQueue;

private:
    int m_width{ 0 };
    int m_height{ 0 };

};