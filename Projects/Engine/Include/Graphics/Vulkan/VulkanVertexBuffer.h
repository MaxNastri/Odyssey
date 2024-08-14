#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "VulkanVertex.h"
#include "ResourceHandle.h"

VK_FWD_DECLARE(VkBuffer)

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;

	class VulkanVertexBuffer : public Resource
	{
	public:
		VulkanVertexBuffer(std::shared_ptr<VulkanContext> context, std::vector<VulkanVertex>& vertices);
		void Destroy();

	public:
		ResourceHandle<VulkanBuffer> GetVertexBuffer();
		const VkBuffer GetVertexBufferVK();
		const VkBuffer* GetVertexBufferVKRef();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceHandle<VulkanBuffer> m_StagingBuffer;
		ResourceHandle<VulkanBuffer> m_VertexBuffer;
	};
}