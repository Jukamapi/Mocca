#include "compute_pipeline.h"

#include "core/vk_check.h"

#include "core/vk_types.h"


ComputePipeline::ComputePipeline(
    const std::string& name,
    VkDevice device,
    const DescriptorLayout& descriptorLayout,
    const std::vector<char>& computeCode
)
    : Pipeline(name, device)
{
    VkShaderModule compShaderModule = createShaderModule(computeCode, device);

    VkPipelineShaderStageCreateInfo compShaderStageInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = compShaderModule,
        .pName = "main",
    };

    VkDescriptorSetLayout setLayout = descriptorLayout.getHandle();

    VkPushConstantRange pushConstant{
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset = 0,
        .size = sizeof(ComputePushConstants),
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &setLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstant,
    };


    VkPipelineLayout localLayout = VK_NULL_HANDLE;
    VkPipeline localPipeline = VK_NULL_HANDLE;
    try
    {
        VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &localLayout));

        VkComputePipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = compShaderStageInfo,
            .layout = localLayout,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &localPipeline));
    }
    catch(...)
    {
        if(localLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, localLayout, nullptr);
        }
        if(localPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, localPipeline, nullptr);
        }

        vkDestroyShaderModule(device, compShaderModule, nullptr);
        throw;
    }

    m_pipelineLayout = localLayout;
    m_pipeline = localPipeline;

    vkDestroyShaderModule(device, compShaderModule, nullptr);
}

ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept
    : Pipeline(std::move(other))
{
    // subclass specifics
}

ComputePipeline& ComputePipeline::operator=(ComputePipeline&& other) noexcept
{
    if(this != &other)
    {
        Pipeline::operator=(std::move(other));
    }
    return *this;
}