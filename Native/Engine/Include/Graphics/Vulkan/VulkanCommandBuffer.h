#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "ResourceManager.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;
	class VulkanCommandPool;
	class VulkanDescriptorBuffer;
	class VulkanGraphicsPipeline;
	class VulkanImage;
	class VulkanIndexBuffer;
	class VulkanVertexBuffer;

	class VulkanCommandBuffer : public Resource
	{
	public:
		VulkanCommandBuffer(std::shared_ptr<VulkanContext> context, ResourceHandle<VulkanCommandPool> commandPool);
		void Destroy(ResourceHandle<VulkanCommandPool> poolHandle);

	public:
		void BeginCommands();
		void EndCommands();
		void Reset();
		void Flush();

	public:
		void BeginRendering(VkRenderingInfoKHR& renderingInfo);
		void EndRendering();
		void BindPipeline(ResourceHandle<VulkanGraphicsPipeline> handle);
		void BindViewport(VkViewport viewport);
		void SetScissor(VkRect2D scissor);
		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
		void TransitionLayouts(VulkanImage* image, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(ResourceHandle<VulkanBuffer> handle, VulkanImage* image, uint32_t width, uint32_t height);
		void BindVertexBuffer(ResourceHandle<VulkanVertexBuffer> handle);
		void CopyBufferToBuffer(ResourceHandle<VulkanBuffer> srcBuffer, ResourceHandle<VulkanBuffer> dstBuffer, uint32_t dataSize);
		void BindIndexBuffer(ResourceHandle<VulkanIndexBuffer> handle);
		void BindDescriptorBuffer(ResourceHandle<VulkanDescriptorBuffer> handle);
		void BindDescriptorBuffers(std::vector<ResourceHandle<VulkanDescriptorBuffer>> handles);
		void SetDescriptorBufferOffset(ResourceHandle<VulkanGraphicsPipeline> graphicsPipeline, uint32_t setIndex, const uint32_t* bufferIndex, const VkDeviceSize* bufferOffset);

	public:
		const VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }
		const VkCommandBuffer* GetCommandBufferRef() { return &m_CommandBuffer; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkCommandBuffer m_CommandBuffer;
	};
}