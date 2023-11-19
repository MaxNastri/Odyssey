#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include <Logger.h>

namespace Odyssey
{
	VulkanBuffer::VulkanBuffer(std::shared_ptr<VulkanContext> context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
	{
		VkDevice device = context->GetDevice()->GetLogicalDevice();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			Logger::LogError("[VulkanBuffer] Failed to create vulkan buffer!");
			return;
		}

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device,buffer, &memoryRequirements);

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
}