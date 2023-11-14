#pragma once
#include <vulkan/vulkan.h>

namespace Odyssey
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool() = default;
		VulkanDescriptorPool(VkDevice logicalDevice);

	public:
		VkDescriptorPool descriptorPool;
	};
}