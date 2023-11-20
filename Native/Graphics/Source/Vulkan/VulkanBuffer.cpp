#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include <Logger.h>

namespace Odyssey
{
	VulkanBuffer::VulkanBuffer(std::shared_ptr<VulkanContext> context, BufferType bufferType, VkDeviceSize size)
	{
		m_Context = context;
		m_BufferType = bufferType;

		VkDevice device = context->GetDevice()->GetLogicalDevice();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = GetUsageFlags(bufferType);
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			Logger::LogError("[VulkanBuffer] Failed to create vulkan buffer!");
			return;
		}

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device,buffer, &memoryRequirements);

		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(context->GetPhysicalDevice()->GetPhysicalDevice(), memoryRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			Logger::LogError("[VulkanBuffer] Failed to allocate buffer memory!");
			return;
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void VulkanBuffer::Destroy()
	{
		VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
		vkFreeMemory(device, bufferMemory, allocator);
		vkDestroyBuffer(device, buffer, allocator);

		bufferMemory = VK_NULL_HANDLE;
		buffer = VK_NULL_HANDLE;
	}

	void VulkanBuffer::SetMemory(VkDeviceSize size, void* data)
	{
		VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
		void* memory;

		// Map, copy and unmap the buffer memory
		vkMapMemory(device, bufferMemory, 0, size, 0, &memory);
		memcpy(memory, data, static_cast<size_t>(size));
		vkUnmapMemory(device, bufferMemory);
	}

	uint32_t VulkanBuffer::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	VkBufferUsageFlags VulkanBuffer::GetUsageFlags(BufferType bufferType)
	{
		switch (bufferType)
		{
			case Odyssey::BufferType::None:
				Logger::LogError("Cannot get usage flags from buffer type: NONE");
				return 0;
			case Odyssey::BufferType::Staging:
				return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			case Odyssey::BufferType::Vertex:
				return VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				break;
			case Odyssey::BufferType::Index:
				return VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}

		return 0;
	}
}