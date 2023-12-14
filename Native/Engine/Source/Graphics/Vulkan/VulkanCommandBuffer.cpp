#include "VulkanCommandBuffer.h"
#include "volk.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "ResourceManager.h"
#include "VulkanDescriptorBuffer.h"

namespace Odyssey
{
	VulkanCommandBuffer::VulkanCommandBuffer(std::shared_ptr<VulkanContext> context, ResourceHandle<VulkanCommandPool> commandPool)
	{
		m_Context = context;

        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = commandPool.Get()->GetCommandPool();
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        VkResult err = vkAllocateCommandBuffers(m_Context->GetDevice()->GetLogicalDevice(), &info, &m_CommandBuffer);
        if (!check_vk_result(err))
        {
            Logger::LogError("(VulkanCommandBuffer) ctor");
        }
	}

    void VulkanCommandBuffer::Destroy(ResourceHandle<VulkanCommandPool> poolHandle)
    {
        VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
        vkFreeCommandBuffers(device, poolHandle.Get()->GetCommandPool(), 1, &m_CommandBuffer);
    }

    void VulkanCommandBuffer::BeginCommands()
    {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VkResult err = vkBeginCommandBuffer(m_CommandBuffer, &begin_info);
        if (!check_vk_result(err))
        {
            Logger::LogError("(commandbuf 2)");
        }
    }

    void VulkanCommandBuffer::EndCommands()
    {
        VkResult err = vkEndCommandBuffer(m_CommandBuffer);
        if (!check_vk_result(err))
        {
            Logger::LogError("(commandbuf 3)");
        }
    }

    void VulkanCommandBuffer::Reset()
    {
        vkResetCommandBuffer(m_CommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }

    void VulkanCommandBuffer::Flush()
    {
        const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &m_CommandBuffer;

        // Create fence to ensure that the command buffer has finished executing
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = 0;
        VkFence fence;
        VkResult err = vkCreateFence(m_Context->GetDeviceVK(), &fenceCreateInfo, nullptr, &fence);
        check_vk_result(err);

        err = vkQueueSubmit(m_Context->GetGraphicsQueueVK(), 1, &end_info, fence);
        check_vk_result(err);

        err = vkWaitForFences(m_Context->GetDeviceVK(), 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
        check_vk_result(err);

        vkDestroyFence(m_Context->GetDeviceVK(), fence, nullptr);
    }

    void VulkanCommandBuffer::BeginRendering(VkRenderingInfoKHR& renderingInfo)
    {
        vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);
    }

    void VulkanCommandBuffer::EndRendering()
    {
        vkCmdEndRendering(m_CommandBuffer);
    }

    void VulkanCommandBuffer::BindPipeline(ResourceHandle<VulkanGraphicsPipeline> handle)
    {
        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, handle.Get()->GetPipeline());
    }

    void VulkanCommandBuffer::BindViewport(VkViewport viewport)
    {
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
    }

    void VulkanCommandBuffer::SetScissor(VkRect2D scissor)
    {
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
    }

    void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        vkCmdDraw(m_CommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
    {
        vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void VulkanCommandBuffer::TransitionLayouts(VulkanImage* image, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkPipelineStageFlags srcStage;
        VkPipelineStageFlags dstStage;
        VkImageMemoryBarrier barrier = VulkanImage::CreateMemoryBarrier(image, oldLayout, newLayout, srcStage, dstStage);
        vkCmdPipelineBarrier(m_CommandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    void VulkanCommandBuffer::CopyBufferToImage(ResourceHandle<VulkanBuffer> handle, VulkanImage* image, uint32_t width, uint32_t height)
    {
        TransitionLayouts(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        VkBufferImageCopy region{};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(m_CommandBuffer, handle.Get()->buffer, image->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        TransitionLayouts(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    void VulkanCommandBuffer::BindVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle)
    {
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, handle.Get()->GetVertexBufferVKRef(), offsets);
    }

    void VulkanCommandBuffer::CopyBufferToBuffer(ResourceHandle<VulkanBuffer> srcBuffer, ResourceHandle<VulkanBuffer> dstBuffer, uint32_t dataSize)
    {
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = dataSize;
        vkCmdCopyBuffer(m_CommandBuffer, srcBuffer.Get()->buffer, dstBuffer.Get()->buffer, 1, &copyRegion);
    }

    void VulkanCommandBuffer::BindIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle)
    {
        vkCmdBindIndexBuffer(m_CommandBuffer, handle.Get()->GetIndexBufferVK(), 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommandBuffer::BindDescriptorBuffer(ResourceHandle<VulkanDescriptorBuffer> handle)
    {
        std::vector<VkDescriptorBufferBindingInfoEXT> bindingInfos;

        VulkanDescriptorBuffer* descriptorBuffer = handle.Get();
        VkDescriptorBufferBindingInfoEXT bindingInfo{};
        bindingInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
        bindingInfo.address = descriptorBuffer->GetBuffer().Get()->GetAddress();
        bindingInfo.usage = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
        bindingInfos.push_back(bindingInfo);

        vkCmdBindDescriptorBuffersEXT(m_CommandBuffer, (uint32_t)bindingInfos.size(), bindingInfos.data());
    }

    void VulkanCommandBuffer::BindDescriptorBuffers(std::vector<ResourceHandle<VulkanDescriptorBuffer>> handles)
    {
        std::vector<VkDescriptorBufferBindingInfoEXT> bindingInfos;

        for (auto& handle : handles)
        {
            VulkanDescriptorBuffer* descriptorBuffer = handle.Get();
            VkDescriptorBufferBindingInfoEXT bindingInfo{};
            bindingInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
            bindingInfo.address = descriptorBuffer->GetBuffer().Get()->GetAddress();
            bindingInfo.usage = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
            bindingInfos.push_back(bindingInfo);
        }

        vkCmdBindDescriptorBuffersEXT(m_CommandBuffer, (uint32_t)bindingInfos.size(), bindingInfos.data());
    }
    void VulkanCommandBuffer::SetDescriptorBufferOffset(ResourceHandle<VulkanGraphicsPipeline> graphicsPipeline, uint32_t setIndex, const uint32_t* bufferIndex, const VkDeviceSize* bufferOffset)
    {
        vkCmdSetDescriptorBufferOffsetsEXT(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.Get()->GetLayout(), setIndex, 1, bufferIndex, bufferOffset);
    }
}