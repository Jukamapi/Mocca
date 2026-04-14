#include "context.h"

#include "engine/window/window.h"

#include <stdexcept>

// get window extensions for instance
// create instance
// use window to get surface

Context::Context(Window& window)
    : m_window{&window}
{
    // initialize instance
    // initialize physical device
    // logical device etc.
}

Context::~Context()
{

}

uint32_t Context::getWindowWidth() const
{
    return m_window->getWidth();
}

uint32_t Context::getWindowHeight() const
{
    return m_window->getHeight();
}