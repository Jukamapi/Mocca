#include "sampler_library.h"
#include "core/vk_check.h"

SamplerLibrary::SamplerLibrary(VkDevice device)
    : m_device(device)
{
    auto createSampler = [this](VkFilter filter, VkSamplerAddressMode mode) -> VkSampler
    {
        VkSamplerCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = filter,
            .minFilter = filter,
            .mipmapMode = (filter == VK_FILTER_LINEAR) ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = mode,
            .addressModeV = mode,
            .addressModeW = mode,
            .maxLod = VK_LOD_CLAMP_NONE,
        };

        VkSampler sampler{VK_NULL_HANDLE};

        VK_CHECK(vkCreateSampler(m_device, &info, nullptr, &sampler));

        return sampler;
    };

    m_linearRepeat = createSampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT);

    m_nearestRepeat = createSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
}

void SamplerLibrary::destroy()
{
    if(m_device != VK_NULL_HANDLE)
    {
        if(m_linearRepeat != VK_NULL_HANDLE)
        {
            vkDestroySampler(m_device, m_linearRepeat, nullptr);
            m_linearRepeat = VK_NULL_HANDLE;
        }
        if(m_nearestRepeat != VK_NULL_HANDLE)
        {
            vkDestroySampler(m_device, m_nearestRepeat, nullptr);
            m_nearestRepeat = VK_NULL_HANDLE;
        }
        m_device = VK_NULL_HANDLE;
    }
}

SamplerLibrary::~SamplerLibrary()
{
    destroy();
}

SamplerLibrary::SamplerLibrary(SamplerLibrary&& other) noexcept
    : m_device(other.m_device),
      m_linearRepeat(other.m_linearRepeat),
      m_nearestRepeat(other.m_nearestRepeat)
{
    other.m_device = VK_NULL_HANDLE;
    other.m_linearRepeat = VK_NULL_HANDLE;
    other.m_nearestRepeat = VK_NULL_HANDLE;
}

SamplerLibrary& SamplerLibrary::operator=(SamplerLibrary&& other) noexcept
{
    if(this != &other)
    {
        destroy();

        m_device = other.m_device;
        m_linearRepeat = other.m_linearRepeat;
        m_nearestRepeat = other.m_nearestRepeat;

        other.m_device = VK_NULL_HANDLE;
        other.m_linearRepeat = VK_NULL_HANDLE;
        other.m_nearestRepeat = VK_NULL_HANDLE;
    }
    return *this;
}