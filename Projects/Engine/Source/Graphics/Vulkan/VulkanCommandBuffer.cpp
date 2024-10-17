#include "VulkanCommandBuffer.h"
#include "volk.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanComputePipeline.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "ResourceManager.h"
#include "VulkanRenderTexture.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanPushDescriptors.h"

namespace Odyssey
{
	VulkanCommandBuffer::VulkanCommandBuffer(ResourceID id, std::shared_ptr<VulkanContext> context, ResourceID commandPoolID)
		: Resource(id)
	{
		m_Context = context;
		m_CommandPool = commandPoolID;

		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_CommandPool);

		// Construct the allocate info
		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.commandPool = commandPool->GetCommandPool();
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		info.commandBufferCount = 1;

		// Allocate the command buffer
		VkResult err = vkAllocateCommandBuffers(m_Context->GetDevice()->GetLogicalDevice(), &info, &m_CommandBuffer);
		if (!check_vk_result(err))
		{
			Log::Error("(VulkanCommandBuffer) ctor");
		}
	}

	void VulkanCommandBuffer::Destroy()
	{
		VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_CommandPool);
		vkFreeCommandBuffers(device, commandPool->GetCommandPool(), 1, &m_CommandBuffer);
	}

	void VulkanCommandBuffer::BeginCommands()
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VkResult err = vkBeginCommandBuffer(m_CommandBuffer, &begin_info);
		if (!check_vk_result(err))
		{
			Log::Error("(commandbuf 2)");
		}
	}

	void VulkanCommandBuffer::EndCommands()
	{
		VkResult err = vkEndCommandBuffer(m_CommandBuffer);
		if (!check_vk_result(err))
		{
			Log::Error("(commandbuf 3)");
		}
	}

	void VulkanCommandBuffer::Reset()
	{
		vkResetCommandBuffer(m_CommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	void VulkanCommandBuffer::SubmitGraphics()
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

	void VulkanCommandBuffer::SubmitCompute()
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

		err = vkQueueSubmit(m_Context->GetComputeQueueVK(), 1, &end_info, fence);
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

	void VulkanCommandBuffer::BindGraphicsPipeline(ResourceID pipelineID)
	{
		auto pipeline = ResourceManager::GetResource<VulkanGraphicsPipeline>(pipelineID);
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
	}

	void VulkanCommandBuffer::BindComputePipeline(ResourceID pipelineID)
	{
		auto pipeline = ResourceManager::GetResource<VulkanComputePipeline>(pipelineID);
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetPipeline());
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

	void VulkanCommandBuffer::TransitionLayouts(ResourceID imageID, VkImageLayout newLayout)
	{
		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;

		auto image = ResourceManager::GetResource<VulkanImage>(imageID);

		if (image->GetLayout() != newLayout)
		{
			VkImageMemoryBarrier barrier = VulkanImage::CreateMemoryBarrier(imageID, image->GetLayout(), newLayout, srcStage, dstStage);
			vkCmdPipelineBarrier(m_CommandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
			image->SetLayout(newLayout);
		}
	}

	void VulkanCommandBuffer::CopyBufferToImage(ResourceID bufferID, ResourceID imageID, uint32_t width, uint32_t height)
	{
		TransitionLayouts(imageID, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		auto buffer = ResourceManager::GetResource<VulkanBuffer>(bufferID);
		auto image = ResourceManager::GetResource<VulkanImage>(imageID);

		auto copyRegions = image->GetCopyRegions();
		vkCmdCopyBufferToImage(m_CommandBuffer, buffer->buffer, image->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (uint32_t)copyRegions.size(), copyRegions.data());

		TransitionLayouts(imageID, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void VulkanCommandBuffer::BindVertexBuffer(ResourceID vertexBufferID)
	{
		VkDeviceSize offsets[] = { 0 };
		auto vertexBuffer = ResourceManager::GetResource<VulkanBuffer>(vertexBufferID);
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &(vertexBuffer->buffer), offsets);
	}

	void VulkanCommandBuffer::CopyBufferToBuffer(ResourceID source, ResourceID destination, size_t dataSize)
	{
		auto srcBuffer = ResourceManager::GetResource<VulkanBuffer>(source);
		auto dstBuffer = ResourceManager::GetResource<VulkanBuffer>(destination);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = dataSize;
		vkCmdCopyBuffer(m_CommandBuffer, srcBuffer->buffer, dstBuffer->buffer, 1, &copyRegion);
	}

	void VulkanCommandBuffer::BindIndexBuffer(ResourceID indexBufferID)
	{
		auto indexBuffer = ResourceManager::GetResource<VulkanBuffer>(indexBufferID);
		vkCmdBindIndexBuffer(m_CommandBuffer, indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanCommandBuffer::PushDescriptorsGraphics(VulkanPushDescriptors* descriptors, ResourceID pipelineID)
	{
		auto pipeline = ResourceManager::GetResource<VulkanGraphicsPipeline>(pipelineID);
		std::vector<VkWriteDescriptorSet> descriptorSets = descriptors->GetWriteDescriptors();

		vkCmdPushDescriptorSetKHR(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetLayout(), 0, (uint32_t)(descriptorSets.size()), descriptorSets.data());
	}

	void VulkanCommandBuffer::PushDescriptorsCompute(VulkanPushDescriptors* descriptors, ResourceID pipelineID)
	{
		auto pipeline = ResourceManager::GetResource<VulkanComputePipeline>(pipelineID);
		std::vector<VkWriteDescriptorSet> descriptorSets = descriptors->GetWriteDescriptors();

		vkCmdPushDescriptorSetKHR(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetLayout(), 0, (uint32_t)(descriptorSets.size()), descriptorSets.data());
	}

	void VulkanCommandBuffer::Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ)
	{
		vkCmdDispatch(m_CommandBuffer, groupX, groupY, groupZ);
	}
}