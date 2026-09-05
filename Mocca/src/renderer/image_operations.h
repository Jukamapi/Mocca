#pragma once

#include <volk.h>
#include <cassert>

// helper method for changing images
inline void transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkAccessFlags2 srcAccess{};
    VkAccessFlags2 dstAccess{};
    VkPipelineStageFlags2 srcStage{};
    VkPipelineStageFlags2 dstStage{};
    VkImageAspectFlags aspectMask{};

    switch(oldLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        srcAccess = VK_ACCESS_2_NONE;
        srcStage = VK_PIPELINE_STAGE_2_NONE;
        break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        srcAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        break;

    case VK_IMAGE_LAYOUT_GENERAL:
        srcAccess = VK_ACCESS_2_SHADER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        break;

    default:
        assert(false && "Old layout type is not supported!");
        break;
    }

    switch(newLayout)
    {
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        break;

    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        dstAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dstStage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT;
        dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        dstAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        break;

    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        dstAccess = VK_ACCESS_2_NONE;
        dstStage = VK_PIPELINE_STAGE_2_NONE;
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        break;

    case VK_IMAGE_LAYOUT_GENERAL:
        // dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        dstAccess = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
        dstStage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        break;


    default:
        assert(false && "New layout type is not supported!");
        break;
    }


    VkImageSubresourceRange range{
        .aspectMask = aspectMask,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkImageMemoryBarrier2 barrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = srcStage,
        .srcAccessMask = srcAccess,
        .dstStageMask = dstStage,
        .dstAccessMask = dstAccess,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = range,
    };

    VkDependencyInfo depInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier,
    };

    vkCmdPipelineBarrier2(cmd, &depInfo);
}


// copies an image from one place to another
// handles scaling, format conversion between high precision to standard
inline void blitImage(VkCommandBuffer cmd, VkImage src, VkExtent2D srcExtent, VkImage dst, VkExtent2D dstExtent)
{
    VkImageBlit2 blitRegion{
        .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
        .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        .srcOffsets = {{0, 0, 0}, {(int32_t)srcExtent.width, (int32_t)srcExtent.height, 1}},
        .dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        .dstOffsets = {{0, 0, 0}, {(int32_t)dstExtent.width, (int32_t)dstExtent.height, 1}},
    };
    VkBlitImageInfo2 blitInfo{
        .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .srcImage = src,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage = dst,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount = 1,
        .pRegions = &blitRegion,
        .filter = VK_FILTER_LINEAR,
    };
    vkCmdBlitImage2(cmd, &blitInfo);
}