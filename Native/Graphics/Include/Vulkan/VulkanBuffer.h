#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkBuffer)
VK_FWD_DECLARE(VkDeviceMemory)

namespace Odyssey
{
	class VulkanContext;

	class VulkanBuffer
	{
	public:
		VulkanBuffer(std::shared_ptr<VulkanContext> context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	public:
		void SetMemory(VkDeviceSize size, void* data);
	private:
		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	public:
		std::shared_ptr<VulkanContext> m_Context;
		VkBuffer buffer = nullptr;
		VkDeviceMemory bufferMemory;
	};
}