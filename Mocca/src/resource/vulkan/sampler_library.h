#pragma once

#include <volk.h>

class SamplerLibrary
{
public:
    SamplerLibrary() = default;
    explicit SamplerLibrary(VkDevice device);
    ~SamplerLibrary();

    SamplerLibrary(const SamplerLibrary&) = delete;
    SamplerLibrary& operator=(const SamplerLibrary&) = delete;
    SamplerLibrary(SamplerLibrary&& other) noexcept;
    SamplerLibrary& operator=(SamplerLibrary&& other) noexcept;

    VkSampler getLinearRepeat() const
    {
        return m_linearRepeat;
    }
    VkSampler getNearestRepeat() const
    {
        return m_nearestRepeat;
    }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkSampler m_linearRepeat{VK_NULL_HANDLE};
    VkSampler m_nearestRepeat{VK_NULL_HANDLE};

    void destroy();
};