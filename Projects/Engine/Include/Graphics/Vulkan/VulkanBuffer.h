#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "Enums.h"
#include "VulkanAllocator.h"

VK_FWD_DECLARE(VkBuffer)
VK_FWD_DECLARE(VkDeviceMemory)

namespace Odyssey
{
	class VulkanContext;

	class VulkanBuffer : public Resource
	{
	public:
		VulkanBuffer(ResourceID id, std::shared_ptr<VulkanContext> context, BufferType bufferType, VkDeviceSize size);
		virtual void Destroy() override;

	public:
		void CopyData(VkDeviceSize size, const void* data);
		void UploadData(const void* data, VkDeviceSize size);
		void CopyBufferMemory(void* dst);

	public:
		uint64_t GetAddress();
		uint32_t GetSize() { return m_Size; }
		VkWriteDescriptorSet GetDescriptorInfo();

	public:
		std::shared_ptr<VulkanContext> m_Context;
		VkBuffer m_Buffer = nullptr;
		VmaAllocation m_MemoryAllocation;
		BufferType m_BufferType;
		uint32_t m_Size;
		VkDescriptorBufferInfo m_Descriptor{};
	};
}