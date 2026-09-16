#pragma once

#include "material_types.h"
#include "renderer/draw_types.h"
#include "vulkan/allocated_buffer.h"


#include <volk.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include <vector>

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

struct GeoSurface
{
    uint32_t startIndex;
    uint32_t count;
    Bounds bounds;
    std::shared_ptr<MaterialInstance> material;
};

struct MeshAsset
{
    std::string name;
    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers meshBuffers;
};