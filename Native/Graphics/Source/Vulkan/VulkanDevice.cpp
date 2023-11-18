#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"

namespace Odyssey
{
	VulkanDevice::VulkanDevice(VulkanPhysicalDevice* physicalDevice)
	{
		CreateLogicalDevice(physicalDevice);
	}

	VulkanDevice::~VulkanDevice()
	{

	}

	void VulkanDevice::WaitForIdle()
	{
		VkResult err = vkDeviceWaitIdle(logicalDevice);
		check_vk_result(err);
	}

	void VulkanDevice::CreateLogicalDevice(VulkanPhysicalDevice* physicalDevice)
	{
		VkPhysicalDevice vkPhysicalDevice = physicalDevice->GetPhysicalDevice();

		std::vector<const char*> device_extensions;
		device_extensions.push_back("VK_KHR_swapchain");
		device_extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
		device_extensions.push_back("VK_KHR_depth_stencil_resolve");
		device_extensions.push_back("VK_KHR_create_renderpass2");
		device_extensions.push_back("VK_KHR_multiview");
		device_extensions.push_back("VK_KHR_maintenance2");

		// Enumerate physical device extension
		uint32_t properties_count;
		std::vector<VkExtensionProperties> properties;
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, nullptr);
		
		properties.resize(properties_count);
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, properties.data());

#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
			device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

		const float queue_priority[] = { 1.0f };
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = physicalDevice->GetFamilyIndex(VulkanQueueType::Graphics);
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;

		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature{};
		dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
		dynamic_rendering_feature.dynamicRendering = VK_TRUE;

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = (uint32_t)device_extensions.size();
		create_info.ppEnabledExtensionNames = device_extensions.data();
		create_info.pNext = &dynamic_rendering_feature;

		VkResult err = vkCreateDevice(vkPhysicalDevice, &create_info, allocator, &logicalDevice);
		check_vk_result(err);
	}
}