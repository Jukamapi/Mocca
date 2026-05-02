#include "renderer.h"

#include "Mocca/vulkan/vk_check.h"
#include "Mocca/vulkan/commands/command_pool.h"
#include "Mocca/vulkan/core/context.h"
#include "Mocca/vulkan/core/physical_device.h"
#include "Mocca/vulkan/resources/swapchain.h"


#include <stdexcept>


Renderer::Renderer(const Context& context, Extent drawableSize)
    : m_context(context), m_frameManager(context.getPhysicalDevice().getQueueFamilyIndices(), context.getDeviceHandle())
{
    m_swapchain = std::make_unique<Swapchain>(
        context.getPhysicalDevice()
            .querySwapChainSupport(context.getPhysicalDeviceHandle(), context.getSurfaceHandle()),
        context.getPhysicalDevice().getQueueFamilyIndices(),
        context.getDeviceHandle(),
        context.getSurfaceHandle(),
        drawableSize
    );

    // const SwapchainSupportDetails &details, const QueueFamilyIndices &indices, VkDevice device, VkSurfaceKHR surface,
    //     Extent frameBufferSize
}

void Renderer::drawFrame()
{
    FrameManager::FrameData& currentFrame = m_frameManager.getCurrentFrame();

    VK_CHECK(vkWaitForFences(m_context.getDeviceHandle(), 1, &currentFrame.renderFence, VK_TRUE, UINT64_MAX));

    currentFrame.deletionQueue.flush();

    currentFrame.commandPool->reset();

    VK_CHECK(vkResetFences(m_context.getDeviceHandle(), 1, &currentFrame.renderFence));

    uint32_t imageIndex;

    // dont use VK_CHECK as it can return VK_ERROR_OUT_OF_DATE_KHR
    VkResult result = vkAcquireNextImageKHR(
        m_context.getDeviceHandle(),
        m_swapchain->getHandle(),
        UINT64_MAX,
        currentFrame.imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain();
        return;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swapchain image!");
    }

    // !!! TODO: Change it so we know which buffer, its hardcoded now !!!
    VkCommandBuffer commandBuffer = currentFrame.commandPool->getBuffers()[0];
    VK_CHECK(vkResetCommandBuffer(commandBuffer, 0));

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    // transition
    VkImageMemoryBarrier imageBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = m_swapchain->getImages()[imageIndex],
        .subresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        }
    };

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageBarrier
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

    // viewport, for example split screen has 2 but 1 render target
    // viewport(image) -> framebuffer
    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        // use swapchains frameBuffer size
        .width = static_cast<float>(m_swapchain->getExtent().width),
        .height = static_cast<float>(m_swapchain->getExtent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // scissor rectangle just cuts away what doesnt fit
    VkRect2D scissor{
        .offset = {0, 0},
        .extent = m_swapchain->getExtent(),
    };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // feature loop
    for(auto& feature : m_features)
    {
        feature->onRender(commandBuffer);
    }

    vkCmdEndRendering(commandBuffer);

    // transition
    VkImageMemoryBarrier presentBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = 0,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = m_swapchain->getImages()[imageIndex],
        .subresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        }
    };

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &presentBarrier
    );

    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &currentFrame.imageAvailableSemaphore,
        .pWaitDstStageMask = &waitStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &currentFrame.renderFinishedSemaphore,
    };

    vkQueueSubmit(m_context.getLogicalDevice().getGraphicsQueue(), 1, &submitInfo, currentFrame.renderFence);

    VkSwapchainKHR swapchainLvalue = m_swapchain->getHandle();
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &currentFrame.renderFinishedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchainLvalue,
        .pImageIndices = &imageIndex,
    };

    // also check event frameBufferResized
    VkResult presentResult = vkQueuePresentKHR(m_context.getLogicalDevice().getPresentQueue(), &presentInfo);

    if(presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
    {
        recreateSwapchain();
    }

    m_frameManager.advance();
}


void Renderer::pushFeature(std::unique_ptr<RenderFeature> feature)
{
    feature->onAttach(m_context.getDeviceHandle(), m_swapchain->getFormat(), m_swapchain->getExtent());

    m_features.push_back(std::move(feature));
}

void Renderer::recreateSwapchain()
{
    vkDeviceWaitIdle(m_context.getDeviceHandle());

    SwapchainSupportDetails details = m_context.getPhysicalDevice().querySwapChainSupport(
        m_context.getPhysicalDeviceHandle(),
        m_context.getSurfaceHandle()
    );

    Extent newExtent{details.capabilities.currentExtent};

    Swapchain newSwapchain{
        details,
        m_context.getPhysicalDevice().getQueueFamilyIndices(),
        m_context.getDeviceHandle(),
        m_context.getSurfaceHandle(),
        newExtent,
        m_swapchain->getHandle()
    };

    *m_swapchain = std::move(newSwapchain);
}

Renderer::~Renderer()
{
    vkDeviceWaitIdle(m_context.getDeviceHandle());
    m_features.clear();
}