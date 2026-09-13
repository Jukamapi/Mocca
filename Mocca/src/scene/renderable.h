#pragma once

#include <glm/mat4x4.hpp>

#include "renderer/draw_types.h"

class IRenderable
{
public:
    virtual ~IRenderable() = default;
    virtual void draw(DrawContext& ctx) = 0;
};