#include "VulkanQueue.h"

namespace Odyssey
{
	VulkanQueue::VulkanQueue(VulkanQueueType type, VkDevice logicalDevice, uint32_t familyIndex)
	{
		queueType = type;
		vkGetDeviceQueue(logicalDevice, familyIndex, 0, &queue);
	}
}