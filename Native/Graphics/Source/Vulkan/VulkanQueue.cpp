#include "VulkanQueue.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"

namespace Odyssey
{
	VulkanQueue::VulkanQueue(VulkanQueueType type, VulkanPhysicalDevice* physicalDevice, VulkanDevice* device)
	{
		queueType = type;
		vkGetDeviceQueue(device->GetLogicalDevice(), 
			physicalDevice->GetFamilyIndex(queueType), 0, &queue);
	}
}