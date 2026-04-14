#include "context.h"
#include "engine/core/window.h"

#include <stdexcept>

Context::Context(Window& window)
    : m_width(), m_height()
{
    if (m_width <= 0 || m_height <= 0) {
        throw std::runtime_error("Invalid window dimensions");
    }

}

Context::~Context()
{

}