#pragma once
#include <vulkan/vulkan.h>

namespace Odyssey
{
	enum VulkanQueueType
	{
		Graphics = 0,
		Compute = 1,
		Transfer = 2,
	};

	class VulkanQueue
	{
	public:
		VulkanQueue() = default;
		VulkanQueue(VulkanQueueType type, VkDevice logicalDevice, uint32_t familyIndex);

	public:
		VulkanQueueType queueType;
		VkQueue queue;
	};
}