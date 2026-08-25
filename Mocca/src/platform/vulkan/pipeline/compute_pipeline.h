#pragma once

#include "pipeline.h"
#include "platform/vulkan/resources/descriptor_layout.h"

#include <string>
#include <vector>


class ComputePipeline : public Pipeline
{
public:
    ComputePipeline() = default;
    ComputePipeline(
        const std::string& name,
        VkDevice device,
        const DescriptorLayout& descriptorLayout,
        const std::vector<char>& computeCode
    );

    ~ComputePipeline() override = default;

    ComputePipeline(const ComputePipeline&) = delete;
    ComputePipeline& operator=(const ComputePipeline&) = delete;
    ComputePipeline(ComputePipeline&& other) noexcept;
    ComputePipeline& operator=(ComputePipeline&& other) noexcept;
};