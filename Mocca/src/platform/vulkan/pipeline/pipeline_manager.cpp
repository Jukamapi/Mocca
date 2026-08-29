#include "pipeline_manager.h"
#include <stdexcept>

PipelineManager::PipelineManager(VkDevice device)
    : m_device(device)
{
    // TODO: load pipeline cache
}

PipelineManager::~PipelineManager()
{
    m_pipelines.clear();
    // TODO: save pipeline cache
    // vkDestroyPipelineCache(m_device, m_pipelineCache, nullptr);
}

ComputePipeline& PipelineManager::createComputePipeline(
    const std::string& name, const DescriptorLayout& layout, std::vector<char> computeCode
)
{
    auto it = m_pipelines.find(name);
    if(it != m_pipelines.end())
    {
        auto* ptr = dynamic_cast<ComputePipeline*>(it->second.get());

        if(!ptr)
        {
            throw std::runtime_error("pipeline '" + name + "' exists but is not a ComputePipeline!");
        }

        return *ptr;
    }

    auto pipeline = std::make_unique<ComputePipeline>(name, m_device, layout, computeCode);

    ComputePipeline& ref = *pipeline;
    m_pipelines[name] = std::move(pipeline);
    return ref;
}

GraphicsPipeline& PipelineManager::createGraphicsPipeline(const std::string& name, const GraphicsPipelineConfig& config)
{
    auto it = m_pipelines.find(name);
    if(it != m_pipelines.end())
    {
        auto* ptr = dynamic_cast<GraphicsPipeline*>(it->second.get());
        if(!ptr)
        {
            throw std::runtime_error("pipeline '" + name + "' exists but is not a GraphicsPipeline!");
        }

        return *ptr;
    }

    auto pipeline = std::make_unique<GraphicsPipeline>(name, m_device, config);

    GraphicsPipeline& ref = *pipeline;
    m_pipelines[name] = std::move(pipeline);
    return ref;
}

Pipeline* PipelineManager::getPipeline(const std::string& name) const
{
    auto it = m_pipelines.find(name);
    return (it != m_pipelines.end()) ? it->second.get() : nullptr;
}