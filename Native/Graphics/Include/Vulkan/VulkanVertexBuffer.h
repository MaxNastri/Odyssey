#pragma once
#include "VulkanGlobals.h"
#include "VulkanVertex.h"

VK_FWD_DECLARE(VkBuffer)

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;

	class VulkanVertexBuffer
	{
	public:
		VulkanVertexBuffer(std::shared_ptr<VulkanContext> context, std::vector<VulkanVertex>& vertices);
		void Destroy();

	public:
		VulkanBuffer* GetVertexBuffer();
		const VkBuffer GetVertexBufferVK();
		const VkBuffer* GetVertexBufferVKRef();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::shared_ptr<VulkanBuffer> m_StagingBuffer;
		std::shared_ptr<VulkanBuffer> m_VertexBuffer;
	};
}