#pragma once
#include "VulkanGlobals.h"

namespace Odyssey
{
	class VulkanContext;
	class VulkanCommandPool;
	class VulkanGraphicsPipeline;
	class VulkanBuffer;
	class VulkanImage;

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(std::shared_ptr<VulkanContext> context, VulkanCommandPool* commandPool);
		void Destroy(VulkanCommandPool* commandPool);

	public:
		void BeginCommands();
		void EndCommands();

	public:
		void BeginRendering(VkRenderingInfoKHR& renderingInfo);
		void EndRendering();
		void BindPipeline(VulkanGraphicsPipeline* pipeline);
		void BindViewport(VkViewport viewport);
		void SetScissor(VkRect2D scissor);
		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		void PipelineBarrier(VkImageMemoryBarrier memoryBarrier, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);
		void CopyBufferToImage(VulkanBuffer* buffer, VulkanImage* image, uint32_t width, uint32_t height);
		void BindVertexBuffer(VulkanBuffer* buffer);
	public:
		const VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }
		const VkCommandBuffer* GetCommandBufferRef() { return &m_CommandBuffer; }
	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkCommandBuffer m_CommandBuffer;
	};
}