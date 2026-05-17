#pragma once

#include <volk.h>
#include <array>
#include <vector>

class DescriptorLayout;

class Pipeline
{
public:
    Pipeline() = default;
    Pipeline(
        VkDevice device,
        VkFormat colorFormat,
        VkFormat depthFormat,
        const DescriptorLayout& descriptorLayout,
        const std::vector<char>& vertCode,
        const std::vector<char>& fragCode
    );
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&) noexcept;
    Pipeline& operator=(Pipeline&&) noexcept;

    VkPipeline getHandle() const
    {
        return m_pipeline;
    }

private:
    VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
    VkPipeline m_pipeline{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};

    static constexpr std::array<VkDynamicState, 2> m_dynamicStates{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device) const;
};