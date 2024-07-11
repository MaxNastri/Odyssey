#pragma once
#include "Enums.h"
#include "VulkanGlobals.h"
#include "Resource.h"
#include "ResourceHandle.h"
#include "VulkanDescriptorSet.h"

VK_FWD_DECLARE(VkDescriptorPool)

namespace Odyssey
{
	class VulkanContext;

	class VulkanDescriptorPool : public Resource
	{
	public:
		VulkanDescriptorPool() = default;
		VulkanDescriptorPool(std::shared_ptr<VulkanContext> context, DescriptorType poolType, uint32_t descriptorCount, uint32_t maxSets);

	public:
		void Destroy();

	public:
		
	public:
		VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkDescriptorPool m_DescriptorPool;
	};
}