#include "renderer.h"

#include "Mocca/vulkan/vk_check.h"
#include "Mocca/vulkan/commands/command_pool.h"
#include "Mocca/vulkan/core/context.h"
#include "Mocca/vulkan/core/physical_device.h"
#include "Mocca/vulkan/resources/swapchain.h"


#include <stdexcept>

// TODO: in future implement RHI, wrap VkCommanBuffer etc in RenderContext class
// but right now Im only supporting vulkan
Renderer::Renderer(const Context& context, ExtentProvider extentProvider)
    : m_context(context), m_extentProvider(std::move(extentProvider)),
      m_frameManager(context.getPhysicalDevice().getQueueFamilyIndices(), context.getDeviceHandle())
{
    m_currentExtent = m_extentProvider();

    m_swapchain = std::make_unique<Swapchain>(
        context.getPhysicalDevice()
            .querySwapChainSupport(context.getPhysicalDeviceHandle(), context.getSurfaceHandle()),
        context.getPhysicalDevice().getQueueFamilyIndices(),
        context.getDeviceHandle(),
        context.getSurfaceHandle(),
        m_currentExtent
    );
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
    if(m_isSwapchainDirty)
    {
        Extent newExtent = m_extentProvider();

        if(newExtent.width == 0 || newExtent.height == 0)
        {
            return false;
        }

        recreateSwapchain(newExtent);
        m_isSwapchainDirty = false;

        return false;
    }

    return true;
}

bool Renderer::acquireNextImage(uint32_t& outImageIndex)
{
    FrameManager::FrameData& currentFrame = m_frameManager.getCurrentFrame();

    VK_CHECK(vkWaitForFences(m_context.getDeviceHandle(), 1, &currentFrame.renderFence, VK_TRUE, UINT64_MAX));

    currentFrame.deletionQueue.flush();

    currentFrame.commandPool.reset();

    VkResult result = vkAcquireNextImageKHR(
        m_context.getDeviceHandle(),
        m_swapchain->getHandle(),
        UINT64_MAX,
        currentFrame.imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &outImageIndex
    );
    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_isSwapchainDirty = true;
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

    VK_CHECK(vkResetFences(m_context.getDeviceHandle(), 1, &currentFrame.renderFence));

    VkCommandBuffer commandBuffer = currentFrame.commandPool.getNextBuffer();

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    // transition to write mode
    transitionImage(
        commandBuffer,
        m_swapchain->getImages()[imageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        0,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
    );

    VkClearValue clearColor = {{{0.01f, 0.01f, 0.01f, 1.0f}}};

    VkRenderingAttachmentInfo attachmentInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = m_swapchain->getImageViews()[imageIndex],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clearColor,
    };

    VkRenderingInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea{
            .offset = {0, 0},
            .extent = m_swapchain->getExtent(),
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo,
        // .pDepthAttachment = &depthAttachmentInfo
    };

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(m_swapchain->getExtent().width),
        .height = static_cast<float>(m_swapchain->getExtent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor{.offset = {0, 0}, .extent = m_swapchain->getExtent()};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // render features
    for(auto& feature : m_features)
    {
        if(feature->isEnabled())
            feature->onRender(commandBuffer);
    }

    vkCmdEndRendering(commandBuffer);

    // transition to present mode
    transitionImage(
        commandBuffer,
        m_swapchain->getImages()[imageIndex],
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        0,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
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
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .deviceIndex = 0,
    };

    // signal semaphore
    VkSemaphoreSubmitInfo signalSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = currentFrame.renderFinishedSemaphore,
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
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


    VK_CHECK(
        vkQueueSubmit2(m_context.getLogicalDevice().getGraphicsQueue(), 1, &submitInfo2, currentFrame.renderFence)
    );

    VkSwapchainKHR swapchainLvalue = m_swapchain->getHandle();
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
        m_isSwapchainDirty = true;
    }
}


void Renderer::pushFeature(std::unique_ptr<RenderFeature> feature)
{
    feature->onAttach(m_context.getDeviceHandle(), m_swapchain->getFormat(), m_swapchain->getExtent());

    m_features.push_back(std::move(feature));
}

void Renderer::recreateSwapchain(Extent newExtent)
{
    m_currentExtent = newExtent;

    vkDeviceWaitIdle(m_context.getDeviceHandle());

    SwapchainSupportDetails details = m_context.getPhysicalDevice().querySwapChainSupport(
        m_context.getPhysicalDeviceHandle(),
        m_context.getSurfaceHandle()
    );

    Swapchain newSwapchain{
        details,
        m_context.getPhysicalDevice().getQueueFamilyIndices(),
        m_context.getDeviceHandle(),
        m_context.getSurfaceHandle(),
        newExtent,
        m_swapchain->getHandle()
    };

    *m_swapchain = std::move(newSwapchain);

    for(auto& feature : m_features)
    {
        feature->onResize(m_currentExtent.width, m_currentExtent.height);
    }
}

void Renderer::transitionImage(
    VkCommandBuffer cmd,
    VkImage image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkAccessFlags2 srcAccess,
    VkAccessFlags2 dstAccess,
    VkPipelineStageFlags2 srcStage,
    VkPipelineStageFlags2 dstStage
)
{
    VkImageAspectFlags aspectMask =
        (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

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

Renderer::~Renderer()
{
    vkDeviceWaitIdle(m_context.getDeviceHandle());
    m_features.clear();
}