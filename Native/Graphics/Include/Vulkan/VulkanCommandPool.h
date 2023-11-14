#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace Odyssey
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(VkDevice device, uint32_t queueIndex);

		VkCommandBuffer AllocateBuffer(VkDevice device);
		void Reset(VkDevice device);
		void Destroy(VkDevice device);

	private:
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}