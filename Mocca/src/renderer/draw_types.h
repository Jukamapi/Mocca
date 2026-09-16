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

struct Bounds
{
    glm::vec3 origin;
    float sphereRadius;
    glm::vec3 extents;
};

struct RenderObject
{
    uint32_t indexCount{0};
    uint32_t firstIndex{0};
    VkBuffer indexBuffer{VK_NULL_HANDLE};

    MaterialInstance* material{nullptr};
    Bounds bounds;
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

struct alignas(16) GlobalRenderData
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambientColor;
    glm::vec4 sunlightDirection;
    glm::vec4 sunlightColor;
};
static_assert(sizeof(GlobalRenderData) % 16 == 0, "GlobalRenderData must be 16-byte aligned");