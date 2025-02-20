#pragma once
#include "vk_mem_alloc.h"

namespace Odyssey
{
	class VulkanContext;

	class VulkanAllocator
	{
	public:
		VulkanAllocator() = default;
		VulkanAllocator(const std::string& tag);
		~VulkanAllocator();

	public:
		VmaAllocation AllocateBuffer(VkBufferCreateInfo createInfo, VmaMemoryUsage usage, bool cpuRead, VkBuffer& outBuffer);
		VmaAllocation AllocateImage(VkImageCreateInfo createInfo, VmaMemoryUsage usage, VkImage& outImage, VkDeviceSize* size = nullptr);
		void Free(VmaAllocation allocation);
		void DestroyImage(VkImage image, VmaAllocation allocation);
		void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);

		template<typename T>
		T* MapMemory(VmaAllocation allocation)
		{
			T* mappedMemory;
			vmaMapMemory(VulkanAllocator::GetAllocator(), allocation, (void**)&mappedMemory);
			return mappedMemory;
		}

		void UnmapMemory(VmaAllocation allocation);

	public:
		static void Init(std::shared_ptr<VulkanContext> context);
		static void Shutdown();
		static VmaAllocator& GetAllocator() { return s_Data->Allocator; }

	private:
		std::string m_Tag;

	private:
		struct VulkanAllocatorData
		{
			VmaAllocator Allocator;
			uint64_t TotalAllocatedBytes = 0;

			uint64_t MemoryUsage = 0; // all heaps
		};
		inline static VulkanAllocatorData* s_Data = nullptr;
	};
}