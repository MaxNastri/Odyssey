#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkCommandBUffer)

namespace Odyssey
{
	class VulkanDevice;

	class VulkanCommandPool
	{
	public:
		VulkanCommandPool() = default;

		VulkanCommandPool(VulkanDevice* device, uint32_t queueIndex);

		VkCommandBuffer AllocateBuffer(VulkanDevice* device);
		void Reset(VulkanDevice* device);
		void Destroy(VulkanDevice* device);

	private:
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}