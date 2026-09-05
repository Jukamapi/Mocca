#include "renderer.h"

#include "core/types.h"
#include "core/vk_check.h"
#include "platform/command_pool.h"
#include "platform/context.h"
#include "platform/context/physical_device.h"
#include "platform/presentation/swapchain.h"


#include <cassert>
#include <stdexcept>


// TODO: IMPORTANT - move transitionImage and blitImage into seperate file

Renderer::Renderer(const Window& window, ExtentProvider extentProvider)
    : m_context(window),
      m_extentProvider(std::move(extentProvider)),
      m_renderExtent(m_extentProvider().width, m_extentProvider().height),
      m_globalDescriptorAllocator(
          m_context.getLogicalDevice().getHandle(),
          128,
          std::array{
              DescriptorAllocator::PoolSizeRatio{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0.5f},
              DescriptorAllocator::PoolSizeRatio{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0.5f},
              DescriptorAllocator::PoolSizeRatio{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0.5f}
          }
      ),
      m_globalUniforms(
          m_context.getLogicalDevice().getHandle(),
          m_context.getVmaAlloc().getVmaAllocator(),
          FrameManager::FRAME_COUNT,
          m_globalDescriptorAllocator
      ),
      m_swapchainManager(
          m_context.getPhysicalDevice(),
          m_context.getLogicalDevice().getHandle(),
          m_context.getSurface().getHandle(),
          m_extentProvider()
      ),
      m_frameManager(m_context.getPhysicalDevice().getQueueFamilyIndices(), m_context.getLogicalDevice().getHandle()),
      m_imGuiManager(m_context, window, m_swapchainManager.getSwapchain(), DRAW_FORMAT, DEPTH_FORMAT),
      m_pipelineManager(m_context.getLogicalDevice().getHandle())
{
    createFrameImages();
}

void Renderer::drawFrame()
{
    if(!processResize())
        return;

    uint32_t imageIndex;
    if(!acquireNextImage(imageIndex))
        return;

    VkCommandBuffer cmd = recordCommandBuffer(imageIndex);

    submitAndPresent(imageIndex, cmd);

    m_frameManager.advance();
}

bool Renderer::processResize()
{
    Extent currentExtent = m_extentProvider();
    ResizeResult result = m_swapchainManager.handleResize(currentExtent);

    if(result == ResizeResult::Recreated)
    {
        m_renderExtent = {currentExtent.width, currentExtent.height};
        destroyFrameImages();
        createFrameImages();

        for(auto& feature : m_features)
        {
            feature->onResize(currentExtent.width, currentExtent.height);
        }
    }

    return result != ResizeResult::Skipped;
}

bool Renderer::acquireNextImage(uint32_t& outImageIndex)
{
    FrameManager::FrameData& currentFrame = m_frameManager.getCurrentFrame();

    VK_CHECK(
        vkWaitForFences(m_context.getLogicalDevice().getHandle(), 1, &currentFrame.renderFence, VK_TRUE, UINT64_MAX)
    );

    currentFrame.deletionQueue.flush();

    currentFrame.frameAllocator.clearPools();

    currentFrame.commandPool.reset();

    VkResult result = vkAcquireNextImageKHR(
        m_context.getLogicalDevice().getHandle(),
        m_swapchainManager.getSwapchain().getHandle(),
        UINT64_MAX,
        currentFrame.imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &outImageIndex
    );
    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_swapchainManager.markDirty();
        return false;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swapchain image!");
    }

    return true;
}

VkCommandBuffer Renderer::recordCommandBuffer(uint32_t imageIndex)
{
    FrameManager::FrameData& currentFrame = m_frameManager.getCurrentFrame();
    Swapchain& swapchain = m_swapchainManager.getSwapchain();

    VkCommandBuffer commandBuffer = currentFrame.commandPool.getNextBuffer();

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    // ---------- compute ----------

    transitionImage(
        commandBuffer,
        currentFrame.colorImage.getImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL
    );

    for(auto& feature : m_features)
    {
        if(feature->isEnabled() && feature->getType() == RenderPassType::Compute)
        {
            feature->onRender(commandBuffer, currentFrame.colorImage.getImageView(), imageIndex);
        }
    }

    // ---------- graphics ----------

    transitionImage(
        commandBuffer,
        currentFrame.colorImage.getImage(),
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    transitionImage(
        commandBuffer,
        currentFrame.depthImage.getImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
    );

    VkRenderingAttachmentInfo colorAttachmentInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = currentFrame.colorImage.getImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD, // set to load so i dont delete the stuff from compute pass
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {{{0.1f, 0.1f, 0.1f, 1.0f}}},
    };

    VkRenderingAttachmentInfo depthAttachmentInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = currentFrame.depthImage.getImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // compute doesnt touch this
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {.depthStencil = {0.f, 0}},
    };

    VkRenderingInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea{
            .offset = {0, 0},
            .extent = m_renderExtent,
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthAttachmentInfo
    };

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(m_renderExtent.width),
        .height = static_cast<float>(m_renderExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{.offset = {0, 0}, .extent = m_renderExtent};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    for(auto& feature : m_features)
    {
        if(feature->isEnabled() && feature->getType() == RenderPassType::Graphics)
        {
            feature->onRender(commandBuffer, currentFrame.colorImage.getImageView(), imageIndex);
        }
    }

    vkCmdEndRendering(commandBuffer);

    // prep color image for blit
    transitionImage(
        commandBuffer,
        currentFrame.colorImage.getImage(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
    );

    // prep swapchain image for blit
    transitionImage(
        commandBuffer,
        swapchain.getImages()[imageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    blitImage(
        commandBuffer,
        currentFrame.colorImage.getImage(),
        m_renderExtent,
        swapchain.getImages()[imageIndex],
        swapchain.getExtent()
    );

    transitionImage(
        commandBuffer,
        swapchain.getImages()[imageIndex],
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );

    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    return commandBuffer;
}

void Renderer::submitAndPresent(uint32_t imageIndex, VkCommandBuffer cmd)
{
    FrameManager::FrameData& currentFrame = m_frameManager.getCurrentFrame();

    // wait semaphore
    VkSemaphoreSubmitInfo waitSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = currentFrame.imageAvailableSemaphore,
        .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        .deviceIndex = 0,
    };

    // signal semaphore
    VkSemaphoreSubmitInfo signalSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = currentFrame.renderFinishedSemaphore,
        .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .deviceIndex = 0,
    };

    VkCommandBufferSubmitInfo cmdSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = cmd,
        .deviceMask = 0,
    };

    VkSubmitInfo2 submitInfo2{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &waitSubmitInfo,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdSubmitInfo,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &signalSubmitInfo,
    };

    VK_CHECK(vkResetFences(m_context.getLogicalDevice().getHandle(), 1, &currentFrame.renderFence));

    VK_CHECK(
        vkQueueSubmit2(m_context.getLogicalDevice().getGraphicsQueue(), 1, &submitInfo2, currentFrame.renderFence)
    );

    VkSwapchainKHR swapchainLvalue = m_swapchainManager.getSwapchain().getHandle();
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &currentFrame.renderFinishedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchainLvalue,
        .pImageIndices = &imageIndex,
    };

    VkResult presentResult = vkQueuePresentKHR(m_context.getLogicalDevice().getPresentQueue(), &presentInfo);

    if(presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
    {
        m_swapchainManager.markDirty();
    }
}


void Renderer::pushFeature(std::unique_ptr<RenderFeature> feature)
{
    m_features.push_back(std::move(feature));
}

void Renderer::transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
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


void Renderer::blitImage(VkCommandBuffer cmd, VkImage src, VkExtent2D srcExtent, VkImage dst, VkExtent2D dstExtent)
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

void Renderer::createFrameImages()
{
    VkExtent3D extent3D = {m_renderExtent.width, m_renderExtent.height, 1};

    for(auto& frame : m_frameManager.getFrames())
    {
        frame.colorImage = AllocatedImage(
            m_context.getLogicalDevice().getHandle(),
            m_context.getVmaAlloc().getVmaAllocator(),
            extent3D,
            DRAW_FORMAT,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
                VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT
        );

        frame.depthImage = AllocatedImage(
            m_context.getLogicalDevice().getHandle(),
            m_context.getVmaAlloc().getVmaAllocator(),
            extent3D,
            DEPTH_FORMAT,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT
        );
    }
}


void Renderer::destroyFrameImages()
{
    for(auto& frame : m_frameManager.getFrames())
    {
        frame.colorImage.destroy();
        frame.depthImage.destroy();
    }
}

Renderer::~Renderer()
{
    vkDeviceWaitIdle(m_context.getLogicalDevice().getHandle());

    m_features.clear();

    destroyFrameImages();
}