#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkDescriptorPool)

namespace Odyssey
{
	class VulkanDevice;

	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool() = default;
		VulkanDescriptorPool(VulkanDevice* device);

	public:
		VkDescriptorPool descriptorPool;
	};
}