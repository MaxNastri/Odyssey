#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanGraphicsPipeline.h"

#include <vulkan/vulkan.h>

namespace Odyssey
{
	VulkanCommandBuffer::VulkanCommandBuffer(std::shared_ptr<VulkanContext> context, VulkanCommandPool* commandPool)
	{
		m_Context = context;

        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = commandPool->GetCommandPool();
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        VkResult err = vkAllocateCommandBuffers(m_Context->GetDevice()->GetLogicalDevice(), &info, &m_CommandBuffer);
        check_vk_result(err);
	}

    void VulkanCommandBuffer::Destroy(VulkanCommandPool* commandPool)
    {
        VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
        vkFreeCommandBuffers(device, commandPool->GetCommandPool(), 1, &m_CommandBuffer);
    }

    void VulkanCommandBuffer::BeginCommands()
    {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VkResult err = vkBeginCommandBuffer(m_CommandBuffer, &begin_info);
        check_vk_result(err);
    }

    void VulkanCommandBuffer::EndCommands()
    {
        VkResult err = vkEndCommandBuffer(m_CommandBuffer);
        check_vk_result(err);
    }

    void VulkanCommandBuffer::BeginRendering(VkRenderingInfoKHR& renderingInfo)
    {
        vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);
    }

    void VulkanCommandBuffer::EndRendering()
    {
        vkCmdEndRendering(m_CommandBuffer);
    }

    void VulkanCommandBuffer::BindPipeline(VulkanGraphicsPipeline* pipeline)
    {
        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
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
    void VulkanCommandBuffer::PipelineBarrier(VkImageMemoryBarrier memoryBarrier, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage)
    {
        vkCmdPipelineBarrier(m_CommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
    }
}