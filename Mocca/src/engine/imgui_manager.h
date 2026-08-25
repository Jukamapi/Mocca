#pragma once

#include "platform/vulkan/commands/command_pool.h"
#include "platform/vulkan/utils/vk_types.h"

#include <volk.h>

#include <functional>

class Context;
class Window;
class Swapchain;

class ImGuiManager
{
public:
    ImGuiManager(const Context& context, const Window& window, const Swapchain& swapchain);
    ~ImGuiManager();

    void submit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
    VkFence m_fence{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
    QueueFamilyIndices m_indices;
    VkQueue m_graphicsQueue{VK_NULL_HANDLE};
    CommandPool m_commandPool;
    VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
};
