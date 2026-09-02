#include "descriptor_writer.h"

DescriptorWriter::DescriptorWriter(VkDevice device)
    : m_device(device)
{
}

DescriptorWriter& DescriptorWriter::writeBuffer(
    int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type
)
{
    VkDescriptorBufferInfo& info =
        m_bufferInfos.emplace_back(VkDescriptorBufferInfo{.buffer = buffer, .offset = offset, .range = size});


    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE, // empty for now
        .dstBinding = static_cast<uint32_t>(binding),
        .descriptorCount = 1,
        .descriptorType = type,
        .pBufferInfo = &info,
    };

    m_writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(
    int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type
)
{
    VkDescriptorImageInfo& info =
        m_imageInfos.emplace_back(VkDescriptorImageInfo{.sampler = sampler, .imageView = image, .imageLayout = layout});

    VkWriteDescriptorSet write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE, // empty for now
        .dstBinding = static_cast<uint32_t>(binding),
        .descriptorCount = 1,
        .descriptorType = type,
        .pImageInfo = &info,
    };

    m_writes.push_back(write);
    return *this;
}

void DescriptorWriter::clear()
{
    m_imageInfos.clear();
    m_writes.clear();
    m_bufferInfos.clear();
}

void DescriptorWriter::updateSet(VkDescriptorSet set)
{
    for(VkWriteDescriptorSet& write : m_writes)
    {
        write.dstSet = set;
    }

    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(m_writes.size()), m_writes.data(), 0, nullptr);
}