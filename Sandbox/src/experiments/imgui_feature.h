#pragma once

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "renderer/render_feature.h"


class ImguiFeature : public RenderFeature
{
public:
    ImguiFeature() = default;

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override
    {
        ImDrawData* drawData = ImGui::GetDrawData();
        if(!drawData)
            return;

        ImGui_ImplVulkan_RenderDrawData(drawData, cmd);
    }

    RenderPassType getType() const override
    {
        return RenderPassType::Graphics;
    }

private:
};