#pragma once

#include <volk.h>
#include <glm/mat4x4.hpp>
#include <vector>

struct MaterialInstance;

struct GPUDrawPushConstants
{
    glm::mat4 worldMatrix;
    VkDeviceAddress vertexBuffer;
};

struct RenderObject
{
    uint32_t indexCount{0};
    uint32_t firstIndex{0};
    VkBuffer indexBuffer{VK_NULL_HANDLE};

    MaterialInstance* material{nullptr};

    glm::mat4 transform{1.0f};
    VkDeviceAddress vertexBufferAddress{0};
};

struct DrawContext
{
    std::vector<RenderObject> opaqueSurfaces;
    std::vector<RenderObject> transparentSurfaces;

    void clear()
    {
        opaqueSurfaces.clear();
        transparentSurfaces.clear();
    }
};