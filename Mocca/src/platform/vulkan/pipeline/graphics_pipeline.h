#pragma once

#include "platform/vulkan/pipeline/pipeline.h"

#include <volk.h>

#include <array>
#include <string>
#include <vector>

class DescriptorLayout;

class GraphicsPipeline : public Pipeline
{
public:
    GraphicsPipeline() = default;
    GraphicsPipeline(
        const std::string& name,
        VkDevice device,
        VkFormat colorFormat,
        VkFormat depthFormat,
        const DescriptorLayout& descriptorLayout,
        const std::vector<char>& vertCode,
        const std::vector<char>& fragCode
    );

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&&) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&&) noexcept;

    ~GraphicsPipeline() override = default;

private:
    inline static constexpr std::array<VkDynamicState, 2> m_dynamicStates{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
};