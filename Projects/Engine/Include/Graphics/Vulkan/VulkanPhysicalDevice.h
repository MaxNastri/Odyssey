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
		void* GetExtensionFeatures() { return last_requested_extension_feature; }
	private:
		void CreatePhysicalDevice(VkInstance instance);
		void FindGraphicsFamilyQueue();
		bool IsDeviceSuitable(VkPhysicalDevice gpu);

	private:

	private:
		VkPhysicalDevice physicalDevice;
		VulkanQueueFamilies indices;
		std::map<VkStructureType, std::shared_ptr<void>> m_ExtensionFeatures;
		void* last_requested_extension_feature{ nullptr };
	};
}