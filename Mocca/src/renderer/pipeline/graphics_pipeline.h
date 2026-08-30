#pragma once

#include "pipeline.h"

#include <volk.h>

#include <array>
#include <string>
#include <vector>

struct GraphicsPipelineConfig
{
    VkFormat colorFormat{VK_FORMAT_UNDEFINED};
    VkFormat depthFormat{VK_FORMAT_UNDEFINED};
    std::vector<char> vertCode;
    std::vector<char> fragCode;

    // layout config
    std::vector<VkDescriptorSetLayout> descriptorLayouts{};
    std::vector<VkPushConstantRange> pushConstants{};

    // fixed states
    VkCullModeFlags cullMode{VK_CULL_MODE_BACK_BIT};
    VkFrontFace frontFace{VK_FRONT_FACE_CLOCKWISE};
    bool enableDepthTest{true};
    bool enableDepthWrite{true};
    bool enableBlending{false};
};

class GraphicsPipeline : public Pipeline
{
public:
    GraphicsPipeline() = default;
    GraphicsPipeline(const std::string& name, VkDevice device, const GraphicsPipelineConfig& config);

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