#pragma once

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "engine/render_feature.h"
#include "engine/renderer.h"


class ImguiFeature : public RenderFeature
{
public:
    ImguiFeature(const Renderer& renderer)
        : m_drawExtent(renderer.getExtent())
    {
    }

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override
    {
        ImDrawData* drawData = ImGui::GetDrawData();
        if(!drawData)
            return;

        VkRenderingAttachmentInfo colorAttachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = drawImageView,
            // TODO: changed from "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL"
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD, // load on top of scene
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };

        VkRenderingInfo renderInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea{
                .offset = {0, 0},
                .extent = m_drawExtent,
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
        };

        vkCmdBeginRendering(cmd, &renderInfo);

        ImGui_ImplVulkan_RenderDrawData(drawData, cmd);

        vkCmdEndRendering(cmd);
    }

private:
    VkExtent2D m_drawExtent{};
};