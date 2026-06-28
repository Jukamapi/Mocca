#include "pipeline.h"

#include "platform/vulkan/utils/vk_check.h"

Pipeline::Pipeline(VkDevice device) {}

Pipeline::Pipeline(Pipeline&& other) noexcept
    : m_pipelineLayout(other.m_pipelineLayout),
      m_pipeline(other.m_pipeline),
      m_device(other.m_device)
{
    other.m_device = VK_NULL_HANDLE;
    other.m_pipeline = VK_NULL_HANDLE;
    other.m_pipelineLayout = VK_NULL_HANDLE;
}

Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
{
    if(this != &other)
    {
        if(m_pipeline != VK_NULL_HANDLE)
            vkDestroyPipeline(m_device, m_pipeline, nullptr);
        if(m_pipelineLayout != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

        m_device = other.m_device;
        m_pipeline = other.m_pipeline;
        m_pipelineLayout = other.m_pipelineLayout;

        other.m_device = VK_NULL_HANDLE;
        other.m_pipeline = VK_NULL_HANDLE;
        other.m_pipelineLayout = VK_NULL_HANDLE;
    }
    return *this;
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code, VkDevice device) const
{
    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data()),
    };
    VkShaderModule shaderModule;
    VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
    return shaderModule;
}

Pipeline::~Pipeline()
{
    if(m_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
    }
    if(m_pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    }
}