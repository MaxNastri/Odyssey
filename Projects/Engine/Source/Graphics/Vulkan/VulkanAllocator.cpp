#include "VulkanAllocator.h"
#include "VulkanContext.h"

namespace Odyssey
{
	VulkanAllocator::VulkanAllocator(const std::string& tag)
		: m_Tag(tag)
	{

	}

	VulkanAllocator::~VulkanAllocator()
	{
	}

	VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo createInfo, VmaMemoryUsage usage, bool cpuRead, VkBuffer& outBuffer)
	{
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = usage;
		if (cpuRead)
			allocInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
		else
			allocInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

		VmaAllocation allocation;
		vmaCreateBuffer(s_Data->Allocator, &createInfo, &allocInfo, &outBuffer, &allocation, nullptr);

		if (allocation == nullptr)
			Log::Error("Failed to allocate buffer");

		return allocation;
	}

	VmaAllocation VulkanAllocator::AllocateImage(VkImageCreateInfo createInfo, VmaMemoryUsage usage, VkImage& outImage, VkDeviceSize* size)
	{
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = usage;

		VmaAllocation allocation;
		vmaCreateImage(s_Data->Allocator, &createInfo, &allocInfo, &outImage, &allocation, nullptr);

		if (allocation == nullptr)
			Log::Error("Failed to allocate image");

		return allocation;
	}

	void VulkanAllocator::Free(VmaAllocation allocation)
	{
		vmaFreeMemory(s_Data->Allocator, allocation);
	}

	void VulkanAllocator::DestroyImage(VkImage image, VmaAllocation allocation)
	{
		vmaDestroyImage(s_Data->Allocator, image, allocation);
	}

	void VulkanAllocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
	{
		vmaDestroyBuffer(s_Data->Allocator, buffer, allocation);
	}

	void VulkanAllocator::UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(s_Data->Allocator, allocation);
	}

	void VulkanAllocator::Init(std::shared_ptr<VulkanContext> context)
	{
		s_Data = new VulkanAllocatorData();

		// Initialize VulkanMemoryAllocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		allocatorInfo.physicalDevice = context->GetPhysicalDeviceVK();
		allocatorInfo.device = context->GetDeviceVK();
		allocatorInfo.instance = context->GetInstance();

		VmaVulkanFunctions vulkanFunctions;
		vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
		vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
		vulkanFunctions.vkAllocateMemory = vkAllocateMemory;
		vulkanFunctions.vkFreeMemory = vkFreeMemory;
		vulkanFunctions.vkMapMemory = vkMapMemory;
		vulkanFunctions.vkUnmapMemory = vkUnmapMemory;
		vulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
		vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
		vulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
		vulkanFunctions.vkBindImageMemory = vkBindImageMemory;
		vulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
		vulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
		vulkanFunctions.vkCreateBuffer = vkCreateBuffer;
		vulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
		vulkanFunctions.vkCreateImage = vkCreateImage;
		vulkanFunctions.vkDestroyImage = vkDestroyImage;
		vulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;
		vulkanFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
		vulkanFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
		vulkanFunctions.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
		vulkanFunctions.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
		vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
		allocatorInfo.pVulkanFunctions = (const VmaVulkanFunctions*)&vulkanFunctions;

		vmaCreateAllocator(&allocatorInfo, &s_Data->Allocator);
	}

	void VulkanAllocator::Shutdown()
	{
		vmaDestroyAllocator(s_Data->Allocator);

		delete s_Data;
		s_Data = nullptr;
	}
}