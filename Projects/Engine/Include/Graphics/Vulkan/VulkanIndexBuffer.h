#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"

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
		ResourceID GetIndexBuffer() { return m_IndexBuffer; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceID m_StagingBuffer;
		ResourceID m_IndexBuffer;
	};
}