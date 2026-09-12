#pragma once

#include <glm/mat4x4.hpp>

#include "renderer/draw_types.h"

class IRenderable
{
public:
    virtual ~IRenderable() = default;
    virtual void draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
};