#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>

#include <string>

class Context;

class Instance
{
public:
    Instance(const std::string& appName, const std::vector<const char*>& extensions);
    ~Instance();
    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;
    Instance(Instance&&) = delete;
    Instance& operator=(Instance&&) = delete;

    bool checkExtensionsSupport(const std::vector<const char*>& requiredExtensions) const;
    bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers) const;
    std::vector<const char*> getRequiredExtensions(const std::vector<const char*>& windowExtensions, uint32_t windowExtensionsCount) const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void setupDebugMessenger();

    VkInstance getInstanceHandle() const { return m_instance; }

private:
    VkInstance m_instance{};
    VkDebugUtilsMessengerEXT m_debugMessenger{};
    bool m_enableValidationLayers{ false };
};