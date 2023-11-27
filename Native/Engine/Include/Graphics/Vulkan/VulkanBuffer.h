#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "Enums.h"

VK_FWD_DECLARE(VkBuffer)
VK_FWD_DECLARE(VkDeviceMemory)

namespace Odyssey
{
	class VulkanContext;

	class VulkanBuffer : public Resource
	{
	public:
		VulkanBuffer(std::shared_ptr<VulkanContext> context, BufferType bufferType, VkDeviceSize size);
		void Destroy();

	public:
		void AllocateMemory();

		void SetMemory(VkDeviceSize size, void* data);
		VkDeviceMemory GetMemory() { return bufferMemory; }
		uint64_t GetAddress();
		uint32_t GetSize() { return m_Size; }
		uint8_t* GetMappedMemory() { return static_cast<uint8_t*>(bufferMemoryMapped); }

	private:
		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkBufferUsageFlags GetUsageFlags(BufferType bufferType);

	public:
		std::shared_ptr<VulkanContext> m_Context;
		VkBuffer buffer = nullptr;
		VkDeviceMemory bufferMemory;
		BufferType m_BufferType;
		uint32_t m_Size;
		void* bufferMemoryMapped = nullptr;
	};
}