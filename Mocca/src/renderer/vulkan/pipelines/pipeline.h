#pragma once

#include <volk.h>
#include <string>
#include <vector>


class Pipeline
{
public:
    Pipeline() = default;
    virtual ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&) noexcept;
    Pipeline& operator=(Pipeline&&) noexcept;

    virtual VkPipeline getHandle() const
    {
        return m_pipeline;
    }

    virtual VkPipelineLayout getLayout() const
    {
        return m_pipelineLayout;
    }

    virtual const std::string& getName() const
    {
        return m_name;
    }

protected:
    const std::string& m_name{"pipeline"};
    VkDevice m_device{VK_NULL_HANDLE};
    VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
    VkPipeline m_pipeline{VK_NULL_HANDLE};

    Pipeline(const std::string& name, VkDevice device);

    VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device) const;
};