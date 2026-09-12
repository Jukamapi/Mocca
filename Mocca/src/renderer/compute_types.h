#pragma once

#include <glm/vec4.hpp>

class ComputePipeline;

struct ComputePushConstants
{
    glm::vec4 data1;
    glm::vec4 data2;
    glm::vec4 data3;
    glm::vec4 data4;
};

struct ComputeEffect
{
    ComputePushConstants data;
    ComputePipeline* pipeline{nullptr};
    const char* name{nullptr};
};