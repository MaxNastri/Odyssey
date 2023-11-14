#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkQueue);

namespace Odyssey
{
	class VulkanDevice;
	class VulkanPhysicalDevice;

	class VulkanQueue
	{
	public:
		VulkanQueue(VulkanQueueType type, VulkanPhysicalDevice* physicalDevice, VulkanDevice* device);

	public:
		VulkanQueueType queueType;
		VkQueue queue;
	};
}