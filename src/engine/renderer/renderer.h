#pragma once

#include "engine/vk/utils.h"

class Context; // forward dec

class Renderer
{
public:

    // needs a reference to the context to create stuff
    Renderer(Context& context);

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void draw();
private:
    Context& m_context;
};