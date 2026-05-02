#include "context.h"

#include "Mocca/platform/window.h"

Context::Context(Window& window)
    : m_instance(window.getAppName(), window.getExtensions()),
      m_surface(window.getNativeWindow(), m_instance.getHandle()),
      m_physicalDevice(m_instance.getHandle(), m_surface.getHandle()),
      m_logicalDevice(
          m_physicalDevice.getHandle(), m_physicalDevice.getQueueFamilyIndices(), m_physicalDevice.getDeviceExtensions()
      )
{
}

Context::~Context() {}