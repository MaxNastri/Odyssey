#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include <Logger.h>
#include "ResourceManager.h"
#include "VulkanCommandPool.h"

namespace Odyssey
{
	inline static VkBufferUsageFlags GetUsageFlags(BufferType bufferType)
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
			case Odyssey::BufferType::Index:
				return VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case BufferType::Uniform:
				return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			case BufferType::Storage:
				return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}

		return 0;
	}

	inline static VkDescriptorType ConvertDescriptorType(BufferType bufferType)
	{
		switch (bufferType)
		{
			case BufferType::None:
				Logger::LogError("[VulkanBuffer] Cannot convert BufferType::None.");
			case BufferType::Uniform:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case BufferType::Storage:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		}

		return (VkDescriptorType)0;
	}

	VulkanBuffer::VulkanBuffer(ResourceID id, std::shared_ptr<VulkanContext> context, BufferType bufferType, VkDeviceSize size)
		: Resource(id)
	{
		m_Context = context;
		m_BufferType = bufferType;
		m_Size = (uint32_t)size;

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

		if (bufferType == BufferType::Uniform || bufferType == BufferType::Storage)
		{
			m_Descriptor.buffer = buffer;
			m_Descriptor.range = VK_WHOLE_SIZE;
			m_Descriptor.offset = 0;
		}

		AllocateMemory();
	}

	void VulkanBuffer::Destroy()
	{
		VkDevice device = m_Context->GetDevice()->GetLogicalDevice();
		vkFreeMemory(device, bufferMemory, allocator);
		vkDestroyBuffer(device, buffer, allocator);

		bufferMemory = VK_NULL_HANDLE;
		buffer = VK_NULL_HANDLE;
	}
	
	void VulkanBuffer::CopyData(VkDeviceSize size, const void* data)
	{
		VkDevice device = m_Context->GetDevice()->GetLogicalDevice();

		if (m_BufferType == BufferType::Uniform)
		{
			memcpy(bufferMemoryMapped, data, static_cast<size_t>(size));
		}
		else
		{
			// Map, copy and unmap the buffer memory
			VkResult err = vkMapMemory(device, bufferMemory, 0, m_Size, 0, &bufferMemoryMapped);

			memcpy(bufferMemoryMapped, data, static_cast<size_t>(size));

			VkMappedMemoryRange range[1] = {};
			range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range[0].memory = bufferMemory;
			range[0].size = m_Size;
			err = vkFlushMappedMemoryRanges(device, 1, range);

			vkUnmapMemory(device, bufferMemory);
		}
	}

	void VulkanBuffer::UploadData(const void* data, VkDeviceSize size)
	{
		// Write the vertices into the staging buffer
		ResourceID stagingBufferID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, size);
		auto stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(stagingBufferID);
		stagingBuffer->CopyData(size, data);

		// Copy the staging buffer into the vertex buffer
		auto commandPool = ResourceManager::GetResource<VulkanCommandPool>(m_Context->GetGraphicsCommandPool());

		ResourceID commandBufferID = commandPool->AllocateBuffer();
		auto commandBuffer = ResourceManager::GetResource<VulkanCommandBuffer>(commandBufferID);

		commandBuffer->BeginCommands();
		commandBuffer->CopyBufferToBuffer(stagingBufferID, m_ResourceID, size);
		commandBuffer->EndCommands();

		commandBuffer->SubmitGraphics();
		commandPool->ReleaseBuffer(commandBufferID);

		ResourceManager::Destroy(stagingBufferID);
	}

	uint64_t VulkanBuffer::GetAddress()
	{
		VkBufferDeviceAddressInfo addressInfo{};
		addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		addressInfo.buffer = buffer;
		return vkGetBufferDeviceAddress(m_Context->GetDeviceVK(), &addressInfo);
	}

	VkWriteDescriptorSet VulkanBuffer::GetDescriptorInfo()
	{
		assert(m_BufferType == BufferType::Uniform || m_BufferType == BufferType::Storage);

		VkWriteDescriptorSet writeSet{};
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = 0;
		writeSet.dstBinding = 0;
		writeSet.descriptorCount = 1;
		writeSet.descriptorType = ConvertDescriptorType(m_BufferType);
		writeSet.pBufferInfo = &m_Descriptor;
		return writeSet;
	}

	void VulkanBuffer::AllocateMemory()
	{
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(m_Context->GetDeviceVK(), buffer, &memoryRequirements);

		m_Size = (uint32_t)memoryRequirements.size;

		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		VkMemoryAllocateFlagsInfo memoryFlags{};
		memoryFlags.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(m_Context->GetPhysicalDeviceVK(), memoryRequirements.memoryTypeBits, properties);

		if (m_BufferType == BufferType::Uniform)
		{
			memoryFlags.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
			allocInfo.pNext = &memoryFlags;
		}

		if (vkAllocateMemory(m_Context->GetDeviceVK(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			Logger::LogError("[VulkanBuffer] Failed to allocate buffer memory!");
			return;
		}

		vkBindBufferMemory(m_Context->GetDeviceVK(), buffer, bufferMemory, 0);

		if (m_BufferType == BufferType::Uniform)
		{
			vkMapMemory(m_Context->GetDeviceVK(), bufferMemory, 0, m_Size, 0, &bufferMemoryMapped);
		}
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