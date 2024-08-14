#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkQueue);

namespace Odyssey
{
	class VulkanContext;
	class VulkanDevice;
	class VulkanPhysicalDevice;

	class VulkanQueue
	{
	public:
		VulkanQueue(VulkanQueueType type, std::shared_ptr<VulkanContext> context);

	public:
		std::shared_ptr<VulkanContext> m_Context;
		VulkanQueueType queueType;
		VkQueue queue;
	};
}