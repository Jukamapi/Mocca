#include "renderer.h"

#include "engine/vk/context.h"

Renderer::Renderer(Context& context)
    : m_context(context)
{
    int width = m_context.getWidth();
    int height = m_context.getHeight();
}


void Renderer::draw()
{
    // nothing rn
}