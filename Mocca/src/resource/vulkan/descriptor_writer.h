#pragma once

#include <volk.h>

#include <deque>
#include <vector>

class DescriptorWriter
{
public:
    explicit DescriptorWriter(VkDevice device);

    DescriptorWriter(const DescriptorWriter&) = delete;
    DescriptorWriter& operator=(const DescriptorWriter&) = delete;
    DescriptorWriter(DescriptorWriter&&) = delete;
    DescriptorWriter& operator=(DescriptorWriter&&) = delete;

    DescriptorWriter& writeImage(
        int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type
    );
    DescriptorWriter& writeBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);

    void clear();
    void updateSet(VkDescriptorSet set);

private:
    VkDevice m_device{VK_NULL_HANDLE};
    std::deque<VkDescriptorImageInfo> m_imageInfos;
    std::deque<VkDescriptorBufferInfo> m_bufferInfos;
    std::vector<VkWriteDescriptorSet> m_writes;
};