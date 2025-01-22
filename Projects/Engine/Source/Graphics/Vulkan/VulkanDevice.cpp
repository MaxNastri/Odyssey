#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"

namespace Odyssey
{
	bool IsExtensionAvailable(std::vector<VkExtensionProperties>& properties, const char* extension)
	{
		for (const VkExtensionProperties& p : properties)
		{
			if (std::strcmp(p.extensionName, extension) == 0)
			{
				return true;
			}
		}

		return false;
	}

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

		if (!check_vk_result(err))
			Log::Error("[VulkanDevice] Failed to wait for idle.");
	}

	void VulkanDevice::CreateLogicalDevice(VulkanPhysicalDevice* physicalDevice)
	{
		VkPhysicalDevice vkPhysicalDevice = physicalDevice->GetPhysicalDevice();

		// Enumerate physical device extension
		uint32_t properties_count;
		std::vector<VkExtensionProperties> properties;
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, nullptr);
		
		properties.resize(properties_count);
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, properties.data());

		std::vector<const char*> device_extensions;
		device_extensions.push_back("VK_KHR_swapchain");

		// Dynamic rendering
		device_extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
		device_extensions.push_back("VK_KHR_depth_stencil_resolve");
		device_extensions.push_back("VK_KHR_create_renderpass2");
		device_extensions.push_back("VK_KHR_multiview");
		device_extensions.push_back("VK_KHR_maintenance2");
		device_extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
		device_extensions.push_back("VK_KHR_bind_memory2");
		device_extensions.push_back("VK_KHR_get_memory_requirements2");

		// Validate our extensions are available
		for (const char* extension : device_extensions)
		{
			if (!IsExtensionAvailable(properties, extension))
			{
				std::string errorMsg = std::format("[VulkanDevice] Extension not supported: {}", extension);
				Log::Error(errorMsg);
				throw std::invalid_argument(errorMsg);
			}
		}

		bool allowUnusedAttachments = false;
		if (IsExtensionAvailable(properties, VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME))
		{
			allowUnusedAttachments = true;
			device_extensions.push_back(VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME);
		}

		const float queue_priority[] = { 1.0f };
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = physicalDevice->GetFamilyIndex(VulkanQueueType::Graphics);
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;

		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering{};
		dynamicRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
		dynamicRendering.dynamicRendering = VK_TRUE;

		VkPhysicalDeviceBufferDeviceAddressFeatures bufferAddress{};
		bufferAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		bufferAddress.bufferDeviceAddress = VK_TRUE;

		dynamicRendering.pNext = &bufferAddress;

		VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT unusedAttachments;

		if (allowUnusedAttachments)
		{
			unusedAttachments.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT;
			unusedAttachments.dynamicRenderingUnusedAttachments = VK_TRUE;
			unusedAttachments.pNext = dynamicRendering.pNext;
			dynamicRendering.pNext = &unusedAttachments;
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.logicOp = true;
		deviceFeatures.samplerAnisotropy = true;
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = (uint32_t)device_extensions.size();
		create_info.ppEnabledExtensionNames = device_extensions.data();
		create_info.pNext = &dynamicRendering;
		create_info.pEnabledFeatures = &deviceFeatures;

		VkResult err = vkCreateDevice(vkPhysicalDevice, &create_info, allocator, &logicalDevice);
		if (!check_vk_result(err))
		{
			std::string errorMsg = "[VulkanDevice] Failed to create vulkan device";
			Log::Error(errorMsg);
			throw std::invalid_argument(errorMsg);
		}

		volkLoadDevice(logicalDevice);
	}
}