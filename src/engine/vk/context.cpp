#include "context.h"

#include "engine/window/window.h"

// get window extensions for instance
// create instance
// use window to get surface

Context::Context(Window& window)
    : m_instance(window.getAppName(), window.getExtensions())
{
    // initialize instance
    // initialize physical device
    // logical device etc.
}

Context::~Context()
{

}
