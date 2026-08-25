#pragma once

#include "platform/vulkan/pipeline/compute_pipeline.h"
#include "platform/vulkan/pipeline/graphics_pipeline.h"
#include "platform/vulkan/resources/descriptor_layout.h"

#include <volk.h>

#include <memory>
#include <string>
#include <unordered_map>


class PipelineManager
{
public:
    PipelineManager(VkDevice device);
    ~PipelineManager();

    PipelineManager(const PipelineManager&) = delete;
    PipelineManager& operator=(const PipelineManager&) = delete;
    PipelineManager(PipelineManager&&) = delete;
    PipelineManager& operator=(PipelineManager&&) = delete;

    // TODO: switch to std::span<const char> instead of vector
    ComputePipeline& createComputePipeline(
        const std::string& name, const DescriptorLayout& layout, std::vector<char> computeCode
    );

    GraphicsPipeline& createGraphicsPipeline(
        const std::string& name,
        VkFormat colorFormat,
        VkFormat depthFormat,
        const DescriptorLayout& descriptorLayout,
        const std::vector<char>& vertCode,
        const std::vector<char>& fragCode
    );

    Pipeline* getPipeline(const std::string& name) const;

private:
    VkDevice m_device{VK_NULL_HANDLE};
    // VkPipelineCache m_pipelineCache{VK_NULL_HANDLE};

    std::unordered_map<std::string, std::unique_ptr<Pipeline>> m_pipelines;
};