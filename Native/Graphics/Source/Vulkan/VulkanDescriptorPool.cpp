#include "VulkanDescriptorPool.h"
#include "VulkanDevice.h"

namespace Odyssey
{
	VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* device)
	{
		VkDescriptorPoolSize pool_sizes[] = {
	{VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
	{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
	{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
	{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
	{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
	{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
	{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
	{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
	{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
	{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
	{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
		};

		uint32_t arrayLength = (uint32_t)(sizeof(pool_sizes) / sizeof(pool_sizes[0]));

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * arrayLength;
		pool_info.poolSizeCount = arrayLength;
		pool_info.pPoolSizes = pool_sizes;
		VkResult err = vkCreateDescriptorPool(device->GetLogicalDevice(), &pool_info, allocator, &descriptorPool);
		check_vk_result(err);
	}
}