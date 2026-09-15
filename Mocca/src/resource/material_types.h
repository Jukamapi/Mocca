#pragma once

#include <volk.h>
#include <cstdint>
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

// TODO MOCCA: add emmissive textures support
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
    bool doubleSided{false};
};