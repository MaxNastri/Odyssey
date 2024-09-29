#pragma once
#include "VulkanGlobals.h"
#include "VulkanCommandBuffer.h"
#include "Resource.h"

VK_FWD_DECLARE(VkCommandPool)

namespace Odyssey
{
	class VulkanContext;
	class VulkanDevice;

	class VulkanCommandPool : public Resource
	{
	public:
		VulkanCommandPool() = default;
		VulkanCommandPool(std::shared_ptr<VulkanContext> context, VulkanQueueType queue);

	public:
		ResourceID AllocateBuffer();
		void ReleaseBuffer(ResourceID commandBuffer);

		void Reset();
		void Destroy();

	public:
		VkCommandPool GetCommandPool() { return commandPool; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkCommandPool commandPool;
		std::vector<ResourceID> commandBuffers;
	};
}