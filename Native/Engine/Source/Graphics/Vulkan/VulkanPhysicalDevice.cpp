#include "VulkanPhysicalDevice.h"
#include <vulkan/vulkan.h>

namespace Odyssey
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance)
	{
		CreatePhysicalDevice(instance);
		FindGraphicsFamilyQueue();
	}

	void VulkanPhysicalDevice::CreatePhysicalDevice(VkInstance instance)
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
			Logger::LogError("[VulkanDevice] Unable to find GPU device");
			physicalDevice = VK_NULL_HANDLE;
		}
	}

	void VulkanPhysicalDevice::FindGraphicsFamilyQueue()
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

	bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties{};
		VkPhysicalDeviceFeatures deviceFeatures{};
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && deviceFeatures.samplerAnisotropy;
	}

	uint32_t VulkanPhysicalDevice::GetFamilyIndex(VulkanQueueType queueType)
	{
		switch (queueType)
		{
			case VulkanQueueType::Graphics:
				return indices.graphicsFamily.value();
			case VulkanQueueType::Compute:
				break;
			case VulkanQueueType::Transfer:
				break;
			default:
				break;
		}

		return std::numeric_limits<uint32_t>::max();
	}
}