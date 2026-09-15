#pragma once

#include "renderer/render_feature.h"

class ImguiFeature : public RenderFeature
{
public:
    ImguiFeature() = default;

    void onRender(VkCommandBuffer cmd, VkImageView drawImageView, uint32_t frameIndex) override;

    RenderPassType getType() const override;

private:
};