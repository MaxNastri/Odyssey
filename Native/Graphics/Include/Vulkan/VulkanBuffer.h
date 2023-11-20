#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkBuffer)
VK_FWD_DECLARE(VkDeviceMemory)

namespace Odyssey
{
	class VulkanContext;

	enum class BufferType
	{
		None = 0,
		Staging = 1,
		Vertex = 2,
		Index = 3,
	};
	class VulkanBuffer
	{
	public:
		VulkanBuffer(std::shared_ptr<VulkanContext> context, BufferType bufferType, VkDeviceSize size);
		void Destroy();

	public:
		void SetMemory(VkDeviceSize size, void* data);
	private:
		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkBufferUsageFlags GetUsageFlags(BufferType bufferType);

	public:
		std::shared_ptr<VulkanContext> m_Context;
		VkBuffer buffer = nullptr;
		VkDeviceMemory bufferMemory;
		BufferType m_BufferType;
	};
}