#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include <Log.h>
#include "ResourceManager.h"
#include "VulkanCommandPool.h"

namespace Odyssey
{
	inline static VkBufferUsageFlags GetUsageFlags(BufferType bufferType)
	{
		switch (bufferType)
		{
			case Odyssey::BufferType::None:
				Log::Error("Cannot get usage flags from buffer type: NONE");
				return 0;
			case Odyssey::BufferType::Staging:
				return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			case Odyssey::BufferType::Vertex:
				return VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			case Odyssey::BufferType::Index:
				return VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case BufferType::Uniform:
				return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
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
				Log::Error("[VulkanBuffer] Cannot convert BufferType::None.");
				break;
			case BufferType::Uniform:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case BufferType::Storage:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		}

		return (VkDescriptorType)0;
	}

	VmaMemoryUsage GetMemoryUsage(BufferType bufferType)
	{
		switch (bufferType)
		{
			case Odyssey::BufferType::None:
				break;
			case Odyssey::BufferType::Staging:
			case Odyssey::BufferType::Uniform:
			case Odyssey::BufferType::Storage:
				return VMA_MEMORY_USAGE_CPU_TO_GPU;
			case Odyssey::BufferType::Vertex:
			case Odyssey::BufferType::Index:
				return VMA_MEMORY_USAGE_GPU_ONLY;
			default:
				break;
		}

		return (VmaMemoryUsage)0;
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

		bool cpuRead = bufferType == BufferType::Storage;

		VulkanAllocator allocator("Buffer");
		m_MemoryAllocation = allocator.AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_AUTO, cpuRead, m_Buffer);

		if (bufferType == BufferType::Uniform || bufferType == BufferType::Storage)
		{
			m_Descriptor.buffer = m_Buffer;
			m_Descriptor.range = VK_WHOLE_SIZE;
			m_Descriptor.offset = 0;
		}
	}

	void VulkanBuffer::Destroy()
	{
		VulkanAllocator allocator("Buffer");
		allocator.DestroyBuffer(m_Buffer, m_MemoryAllocation);

		m_Buffer = nullptr;
		m_MemoryAllocation = nullptr;
	}

	void VulkanBuffer::CopyData(VkDeviceSize size, const void* data)
	{
		VulkanAllocator allocator("Buffer");

		uint8_t* memData = allocator.MapMemory<uint8_t>(m_MemoryAllocation);
		memcpy(memData, data, (size_t)size);
		allocator.UnmapMemory(m_MemoryAllocation);
	}

	void VulkanBuffer::UploadData(const void* data, VkDeviceSize size)
	{
		// Write the vertices into the staging buffer
		ResourceID stagingBufferID = ResourceManager::Allocate<VulkanBuffer>(BufferType::Staging, size);
		Ref<VulkanBuffer> stagingBuffer = ResourceManager::GetResource<VulkanBuffer>(stagingBufferID);
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

	void VulkanBuffer::CopyBufferMemory(void* dst)
	{
		VulkanAllocator allocator("Buffer");

		uint8_t* memData = allocator.MapMemory<uint8_t>(m_MemoryAllocation);
		memcpy(dst, memData, (size_t)m_Size);
		allocator.UnmapMemory(m_MemoryAllocation);
	}

	uint64_t VulkanBuffer::GetAddress()
	{
		VkBufferDeviceAddressInfo addressInfo{};
		addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		addressInfo.buffer = m_Buffer;
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
}