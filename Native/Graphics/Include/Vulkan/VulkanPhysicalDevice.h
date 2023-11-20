#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkInstance)
VK_FWD_DECLARE(VkPhysicalDevice)

namespace Odyssey
{
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(VkInstance instance);

	public:
		VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }
		uint32_t GetFamilyIndex(VulkanQueueType queueType);

	private:
		void CreatePhysicalDevice(VkInstance instance);
		void FindGraphicsFamilyQueue();
		bool IsDeviceSuitable(VkPhysicalDevice gpu);

	private:

		VkPhysicalDevice physicalDevice;
		VulkanQueueFamilies indices;
	};
}