#pragma once
#include <vma/vk_mem_alloc.h>

// this class handles the creation of VmaAllocator and integrating volk into it
class VmaAlloc
{
public:
    VmaAlloc(VkPhysicalDevice physicalDevice, VkDevice device, VkInstance instance);
    ~VmaAlloc();

    VmaAlloc(const VmaAlloc&) = delete;
    VmaAlloc& operator=(const VmaAlloc&) = delete;
    VmaAlloc(VmaAlloc&&) = delete;
    VmaAlloc& operator=(VmaAlloc&&) = delete;

    const VmaAllocator& getVmaAllocator() const
    {
        return m_allocator;
    }

private:
    VmaAllocator m_allocator;
};