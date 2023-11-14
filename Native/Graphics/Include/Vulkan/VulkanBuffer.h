#pragma once
#include <vulkan/vulkan.h>

namespace Odyssey
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	private:
		uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	public:
		VkBuffer* buffer = nullptr;
		VkDeviceMemory* bufferMemory = nullptr;
	};
}