#pragma once

#include <vector>
#include <volk.h>


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

    VkInstance getHandle() const
    {
        return m_instance;
    }

private:
    bool checkExtensionsSupport(const std::vector<const char*>& requiredExtensions) const;

    bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers) const;

    std::vector<const char*> getRequiredExtensions(const std::vector<const char*>& windowExtensions) const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;

    void setupDebugMessenger();


    VkInstance m_instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};
    bool m_enableValidationLayers{false};
    std::vector<const char*> m_validationLayers{"VK_LAYER_KHRONOS_validation"};
};