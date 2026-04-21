#include "engine/vk/frame_manager.h"

FrameManager::FrameManager()
    : m_currentFrameIndex(0)
{
    //createCommandPools later
}

void FrameManager::advance()
{
    m_currentFrameIndex = (m_currentFrameIndex + 1) % 2;
}

FrameManager::~FrameManager()
{
    //destroyCommandPools later
}