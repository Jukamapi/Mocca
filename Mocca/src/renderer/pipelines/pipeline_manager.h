#pragma once

#include "compute_pipeline.h"
#include "graphics_pipeline.h"
#include "resource/vulkan/descriptor_layout.h"

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

    ComputePipeline& createComputePipeline(
        const std::string& name, const DescriptorLayout& layout, std::vector<char> computeCode
    );

    GraphicsPipeline& createGraphicsPipeline(const std::string& name, const GraphicsPipelineConfig& config);

    Pipeline* getPipeline(const std::string& name) const;

private:
    VkDevice m_device{VK_NULL_HANDLE};
    // VkPipelineCache m_pipelineCache{VK_NULL_HANDLE};

    std::unordered_map<std::string, std::unique_ptr<Pipeline>> m_pipelines;
};