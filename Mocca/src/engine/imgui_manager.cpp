#include "imgui_manager.h"

#include "platform/vulkan/utils/vk_check.h"

#include "platform/vulkan/core/context.h"
#include "platform/vulkan/resources/swapchain.h"
#include "platform/window.h"


#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"


// TODO: add this code to run() function in rendering loop
ImGuiManager::ImGuiManager(const Context& context, const Window& window, const Swapchain& swapchain)
    : m_device(context.getLogicalDevice().getHandle()),
      m_graphicsQueue(context.getLogicalDevice().getGraphicsQueue()),
      m_commandPool(context.getPhysicalDevice().getQueueFamilyIndices(), m_device)
{
    VkFenceCreateInfo fenceInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    try
    {
        m_commandPool.allocateBuffers(1);
    }
    catch(...)
    {
        throw;
    }

    VK_CHECK(vkCreateFence(m_device, &fenceInfo, nullptr, &m_fence));

    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };

    VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1000,
        .poolSizeCount = (uint32_t)std::size(pool_sizes),
        .pPoolSizes = pool_sizes
    };

    VkDescriptorPool imguiPool;
    VK_CHECK(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &imguiPool));

    // beginning of imgui initialization
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

    ImGui_ImplSDL2_InitForVulkan(window.getNativeWindow());

    VkFormat lFormat = swapchain.getFormat();
    ImGui_ImplVulkan_InitInfo initInfo{
        .Instance = context.getInstance().getHandle(),
        .PhysicalDevice = context.getPhysicalDevice().getHandle(),
        .Device = m_device,
        .Queue = m_graphicsQueue,
        .DescriptorPool = imguiPool,
        .MinImageCount = 3,
        .ImageCount = 3,
        .UseDynamicRendering = true,
    };
    // wouldve used nested designators, but it's C exclusive?
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &lFormat,
    };
    initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&initInfo);
}

void ImGuiManager::submit(std::function<void(VkCommandBuffer cmd)>&& function)
{
    VK_CHECK(vkResetFences(m_device, 1, &m_fence));
    m_commandPool.reset();

    VkCommandBuffer cmd = m_commandPool.getBuffers()[m_commandPool.getBuffersCount()];

    VkCommandBufferBeginInfo cmdBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    function(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    VkCommandBufferSubmitInfo cmdSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = cmd
    };

    VkSubmitInfo2 submit{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2, .pCommandBufferInfos = &cmdSubmitInfo};

    VK_CHECK(vkQueueSubmit2(m_graphicsQueue, 1, &submit, m_fence));
    VK_CHECK(vkWaitForFences(m_device, 1, &m_fence, true, 9999999999));
}

ImGuiManager::~ImGuiManager()
{
    if(m_fence != VK_NULL_HANDLE)
    {
        vkDestroyFence(m_device, m_fence, nullptr);
    }
    ImGui_ImplVulkan_Shutdown();
    if(m_descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
    };
}
