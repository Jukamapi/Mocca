#pragma once

#include "renderer/pipelines/compute_pipeline.h"
#include "resource/vulkan/allocated_buffer.h"

#include <volk.h>

#include <memory>
#include <optional>
#include <vector>


#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

enum class MaterialPass : uint8_t
{
    MainColor,
    Transparent,
    Other
};


struct alignas(16) MaterialConstants
{
    glm::vec4 colorFactors{1.0f};
    glm::vec4 metalRoughFactors{1.0f, 0.5f, 0.0f, 0.0f};
    glm::vec4 extra[14]{};
};
static_assert(sizeof(MaterialConstants) == 256, "MaterialConstants must be 256 bytes");

struct MaterialResources
{
    VkImageView colorImageView{VK_NULL_HANDLE};
    VkSampler colorSampler{VK_NULL_HANDLE};
    VkImageView metalRoughImageView{VK_NULL_HANDLE};
    VkSampler metalRoughSampler{VK_NULL_HANDLE};
    VkBuffer dataBuffer{VK_NULL_HANDLE};
    uint32_t dataBufferOffset{0};
};

struct MaterialInstance
{
    VkDescriptorSet materialSet{VK_NULL_HANDLE};
    MaterialPass passType{MaterialPass::MainColor};
};


struct ComputePushConstants
{
    glm::vec4 data1;
    glm::vec4 data2;
    glm::vec4 data3;
    glm::vec4 data4;
};

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct ComputeEffect
{
    ComputePushConstants data;

    ComputePipeline* pipeline;

    const char* name;
};

struct Vertex
{
    glm::vec3 position;
    float uv_x;
    glm::vec3 normal;
    float uv_y;
    glm::vec4 color;
};

struct GPUMeshBuffers
{
    AllocatedBuffer indexBuffer;
    AllocatedBuffer vertexBuffer;
    VkDeviceAddress vertexBufferAddress;
};

struct GPUDrawPushConstants
{
    glm::mat4 worldMatrix;
    VkDeviceAddress vertexBuffer;
};

struct GeoSurface
{
    uint32_t startIndex;
    uint32_t count;
    std::shared_ptr<MaterialInstance> material;
};

struct MeshAsset
{
    std::string name;

    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers meshBuffers;
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

/*
class IRenderable {

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
};
*/