#include "pipeline.h"

Pipeline::Pipeline(VkDevice device) {}

Pipeline::Pipeline(Pipeline&& other) noexcept
    : m_pipelineLayout(other.m_pipelineLayout), m_pipeline(other.m_pipeline), m_device(other.m_device)
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