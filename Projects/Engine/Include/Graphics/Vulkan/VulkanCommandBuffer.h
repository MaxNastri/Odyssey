#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "ResourceManager.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;
	class VulkanCommandPool;
	class VulkanDescriptorSet;
	class VulkanDescriptorLayout;
	class VulkanGraphicsPipeline;
	class VulkanImage;
	class VulkanIndexBuffer;
	class VulkanVertexBuffer;
	class VulkanUniformBuffer;
	class VulkanPushDescriptors;

	class VulkanCommandBuffer : public Resource
	{
	public:
		VulkanCommandBuffer(std::shared_ptr<VulkanContext> context, ResourceID commandPoolID);
		virtual void Destroy() override;

	public:
		void BeginCommands();
		void EndCommands();
		void Reset();
		void Flush();

	public:
		void BeginRendering(VkRenderingInfoKHR& renderingInfo);
		void EndRendering();
		void BindPipeline(ResourceID pipelineID);
		void BindViewport(VkViewport viewport);
		void SetScissor(VkRect2D scissor);
		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
		void TransitionLayoutsRT(ResourceID renderTextureID, VkImageLayout newLayout);
		void TransitionLayouts(ResourceID imageID, VkImageLayout newLayout);
		void CopyBufferToImage(ResourceID bufferID, ResourceID imageID, uint32_t width, uint32_t height);
		void BindVertexBuffer(ResourceID vertexBufferID);
		void CopyBufferToBuffer(ResourceID source, ResourceID destination, uint32_t dataSize);
		void BindIndexBuffer(ResourceID indexBufferID);
		void BindDescriptorSet(ResourceID descriptorSetID, ResourceID pipelineID);
		void PushDescriptors(VulkanPushDescriptors* descriptors, ResourceID pipelineID);
	public:
		const VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }
		const VkCommandBuffer* GetCommandBufferRef() { return &m_CommandBuffer; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceID m_CommandPool;
		VkCommandBuffer m_CommandBuffer;
	};
}