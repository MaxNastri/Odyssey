#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkBuffer)

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;

	class VulkanIndexBuffer
	{
	public:
		VulkanIndexBuffer(std::shared_ptr<VulkanContext> context, std::vector<uint32_t>& indices);
		void Destroy();

	public:
		VulkanBuffer* GetIndexBuffer();
		const VkBuffer GetIndexBufferVK();
		const VkBuffer* GetIndexBufferVKRef();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::shared_ptr<VulkanBuffer> m_StagingBuffer;
		std::shared_ptr<VulkanBuffer> m_IndexBuffer;
	};
}