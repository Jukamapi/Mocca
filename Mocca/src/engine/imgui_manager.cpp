#include "imgui_manager.h"

#include "platform/vulkan/utils/vk_check.h"

#include "platform/vulkan/core/context.h"
#include "platform/vulkan/resources/swapchain.h"
#include "platform/window.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

ImGuiManager::ImGuiManager(
    const Context& context, const Window& window, const Swapchain& swapchain, VkFormat colorFormat, VkFormat depthFormat
)
    : m_device(context.getLogicalDevice().getHandle())
{

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

    VK_CHECK(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool));

    // beginning of imgui initialization
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

    ImGui_ImplSDL2_InitForVulkan(window.getNativeWindow());

    ImGui_ImplVulkan_InitInfo initInfo{
        .Instance = context.getInstance().getHandle(),
        .PhysicalDevice = context.getPhysicalDevice().getHandle(),
        .Device = m_device,
        .Queue = context.getLogicalDevice().getGraphicsQueue(),
        .DescriptorPool = m_descriptorPool,
        .MinImageCount = 3,
        .ImageCount = 3,
        .PipelineInfoMain =
            {
                .MSAASamples = VK_SAMPLE_COUNT_1_BIT,

                .PipelineRenderingCreateInfo =
                    {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
                     .colorAttachmentCount = 1,
                     .pColorAttachmentFormats = &colorFormat,
                     .depthAttachmentFormat = depthFormat},
            },
        .UseDynamicRendering = true,

    };

    ImGui_ImplVulkan_Init(&initInfo);
}


void ImGuiManager::beginFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
}

void ImGuiManager::endFrame()
{
    ImGui::Render();
}

ImGuiManager::~ImGuiManager()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    if(m_descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
    };
}
