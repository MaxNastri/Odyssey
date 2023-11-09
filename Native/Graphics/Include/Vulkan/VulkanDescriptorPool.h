#pragma once
#include <vulkan/vulkan.h>

namespace Odyssey::Graphics
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool() = default;
		VulkanDescriptorPool(VkDevice logicalDevice);

	private:
		VkDescriptorPool descriptorPool;
	};
}