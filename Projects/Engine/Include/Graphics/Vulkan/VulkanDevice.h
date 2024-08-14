#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkDevice)

namespace Odyssey
{
	class VulkanContext;
	class VulkanPhysicalDevice;

	class VulkanDevice
	{
	public:
		VulkanDevice(VulkanPhysicalDevice* physicalDevice);
		~VulkanDevice();

	public:
		void WaitForIdle();

	public:
		VkDevice GetLogicalDevice() { return logicalDevice; }

	private:
		void CreateLogicalDevice(VulkanPhysicalDevice* physicalDevice);

	private:
		VkDevice logicalDevice;
	};
}