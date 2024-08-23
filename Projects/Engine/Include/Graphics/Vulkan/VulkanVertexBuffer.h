#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "Vertex.h"

VK_FWD_DECLARE(VkBuffer)

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;

	class VulkanVertexBuffer : public Resource
	{
	public:
		VulkanVertexBuffer(std::shared_ptr<VulkanContext> context, std::vector<Vertex>& vertices);
		void Destroy();

	public:
		ResourceID GetBuffer();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		ResourceID m_StagingBuffer;
		ResourceID m_VertexBuffer;
	};
}