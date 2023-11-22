#pragma once
#include "VulkanGlobals.h"
#include "ResourceManager.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;
	class VulkanCommandPool;
	class VulkanGraphicsPipeline;
	class VulkanImage;
	class VulkanIndexBuffer;
	class VulkanVertexBuffer;

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(std::shared_ptr<VulkanContext> context, VulkanCommandPool* commandPool);
		void Destroy(VulkanCommandPool* commandPool);

	public:
		void BeginCommands();
		void EndCommands();
		void Reset();

	public:
		void BeginRendering(VkRenderingInfoKHR& renderingInfo);
		void EndRendering();
		void BindPipeline(VulkanGraphicsPipeline* pipeline);
		void BindViewport(VkViewport viewport);
		void SetScissor(VkRect2D scissor);
		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
		void TransitionLayouts(VulkanImage* image, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(VulkanBuffer* buffer, VulkanImage* image, uint32_t width, uint32_t height);
		void BindVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle);
		void CopyBufferToBuffer(VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer, uint32_t dataSize);
		void BindIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle);

	public:
		const VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }
		const VkCommandBuffer* GetCommandBufferRef() { return &m_CommandBuffer; }

	private:
		VkImageMemoryBarrier CreateMemoryBarrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t& sourceStage, uint32_t& destStage);
	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkCommandBuffer m_CommandBuffer;
	};
}