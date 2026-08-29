#pragma once

#include <volk.h>

#include <functional>

class Context;
class Window;
class Swapchain;

class ImGuiManager
{
public:
    ImGuiManager(
        const Context& context,
        const Window& window,
        const Swapchain& swapchain,
        VkFormat colorFormat,
        VkFormat depthFormat
    );
    ~ImGuiManager();

    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;

    ImGuiManager(ImGuiManager&&) = delete;
    ImGuiManager& operator=(ImGuiManager&&) = delete;

    void submit(std::function<void(VkCommandBuffer cmd)>&& function);

    void beginFrame();
    void endFrame();

private:
    VkRenderingAttachmentInfo attachmentInfo(VkImageView view, VkClearValue* clear, VkImageLayout layout);

    VkDevice m_device{VK_NULL_HANDLE};
    VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
};
