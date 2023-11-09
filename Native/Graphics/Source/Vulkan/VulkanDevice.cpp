#include "VulkanDevice.h"
#include "VulkanGlobals.h"
#include <assert.h>
#include <vector>

namespace Odyssey::Graphics
{
	VulkanDevice::VulkanDevice(VkInstance instance)
	{
		CreatePhysicalDevice(instance);
		FindGraphicsQueueFamily();
		CreateLogicalDevice();
	}

	VulkanDevice::~VulkanDevice()
	{

	}

	uint32_t VulkanDevice::GetFamilyIndex(VulkanQueueType queueType)
	{
		switch (queueType)
		{
		case Odyssey::Graphics::Graphics:
			return indices.graphicsFamily.value();
		case Odyssey::Graphics::Compute:
			break;
		case Odyssey::Graphics::Transfer:
			break;
		default:
			break;
		}

		return 0;
	}

	void VulkanDevice::CreatePhysicalDevice(VkInstance instance)
	{
		// Get the number of GPUs
		uint32_t gpu_count;
		VkResult err = vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);
		check_vk_result(err);
		assert(gpu_count > 0);

		// Get the GPUs
		std::vector<VkPhysicalDevice> gpus;
		gpus.resize(gpu_count);
		err = vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());
		check_vk_result(err);

		// Find the descrete GPU, if it exists
		for (VkPhysicalDevice& device : gpus)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				physicalDevice = device;
				break;
			}
		}

		// Use first GPU (Integrated) if a discrete one is not available.
		if (physicalDevice == VK_NULL_HANDLE && gpu_count > 0)
		{
			physicalDevice = gpus[0];
		}

		if (physicalDevice == VK_NULL_HANDLE)
		{
			// Coulnd't find any GPU
			Framework::Log::Error("[VulkanDevice] Unable to find GPU device");
			physicalDevice = VK_NULL_HANDLE;
		}
	}

	void VulkanDevice::FindGraphicsQueueFamily()
	{
		// Get the number of queue families
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

		// Get the queues
		VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queues);

		// Find the graphics queue
		for (uint32_t i = 0; i < count; i++)
		{
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
				break;
			}
		}

		free(queues);
		assert(indices.graphicsFamily.has_value());
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		std::vector<const char*> device_extensions;
		device_extensions.push_back("VK_KHR_swapchain");

		// Enumerate physical device extension
		uint32_t properties_count;
		std::vector<VkExtensionProperties> properties;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &properties_count, nullptr);
		properties.resize(properties_count);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &properties_count, properties.data());
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
			device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

		const float queue_priority[] = { 1.0f };
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = indices.graphicsFamily.value();
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = (uint32_t)device_extensions.size();
		create_info.ppEnabledExtensionNames = device_extensions.data();

		VkResult err = vkCreateDevice(physicalDevice, &create_info, allocator, &logicalDevice);
		check_vk_result(err);
	}
}