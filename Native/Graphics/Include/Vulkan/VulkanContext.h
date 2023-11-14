#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkInstance)
VK_FWD_DECLARE(VkDebugReportCallbackEXT)

struct VkExtensionProperties;

namespace Odyssey
{
	class VulkanDevice;
	class VulkanPhysicalDevice;

	class VulkanContext
	{
	public:
		VulkanContext();

		VkInstance GetInstance() { return instance; }
		VulkanPhysicalDevice* GetPhysicalDevice() { return physicalDevice.get(); }
		VulkanDevice* GetDevice() { return logicalDevice.get(); }

	private:
		void GatherExtensions();
		void CreateInstance();

		bool IsExtensionAvailable(std::vector<VkExtensionProperties>& properties, const char* extension);
	private: // Vulkan objects
		VkInstance instance = nullptr;
		std::shared_ptr<VulkanPhysicalDevice> physicalDevice;
		std::shared_ptr<VulkanDevice> logicalDevice;

	private: // Extensions
		std::vector<const char*> extensions;
		uint32_t extensionsCount = 0;

	private: // Debugging
		bool enableValidation = true;
		VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
	};
}