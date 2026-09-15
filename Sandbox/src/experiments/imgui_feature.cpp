#include "imgui_feature.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

void ImguiFeature::onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex)
{
    ImDrawData* drawData = ImGui::GetDrawData();
    if(!drawData)
        return;

    ImGui_ImplVulkan_RenderDrawData(drawData, cmd);
}

RenderPassType ImguiFeature::getType() const
{
    return RenderPassType::Graphics;
}