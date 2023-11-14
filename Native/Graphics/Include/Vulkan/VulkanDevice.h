#pragma once
#include <optional>
#include <vulkan/vulkan.h>
#include "VulkanQueue.h"

namespace Odyssey
{
	class VulkanDevice
	{
	public:
		VulkanDevice() = default;
		VulkanDevice(VkInstance instance);
		~VulkanDevice();

	public:
		VkDevice GetLogicalDevice() { return logicalDevice; }
		VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }
		uint32_t GetFamilyIndex(VulkanQueueType queueType);

	private:
		void CreatePhysicalDevice(VkInstance instance);
		void FindGraphicsQueueFamily();
		void CreateLogicalDevice();

	private:
		struct VulkanQueueFamilies
		{
			std::optional<uint32_t> graphicsFamily;
		};

	private:
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;

	private:
		VulkanQueueFamilies indices;
	};
}