#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "ResourceHandle.h"

VK_FWD_DECLARE(VkBuffer)

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;

	class VulkanIndexBuffer : public Resource
	{
	public:
		VulkanIndexBuffer(std::shared_ptr<VulkanContext> context, std::vector<uint32_t>& indices);
		void Destroy();

	public:
		ResourceHandle<VulkanBuffer> GetIndexBuffer();
		const VkBuffer GetIndexBufferVK();
		const VkBuffer* GetIndexBufferVKRef();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceHandle<VulkanBuffer> m_StagingBuffer;
		ResourceHandle<VulkanBuffer> m_IndexBuffer;
	};
}