#pragma once
#include "Enums.h"
#include "VulkanGlobals.h"
#include "Resource.h"
#include "VulkanDescriptorSet.h"

VK_FWD_DECLARE(VkDescriptorPool)

namespace Odyssey
{
	class VulkanContext;

	class VulkanDescriptorPool : public Resource
	{
	public:
		VulkanDescriptorPool(ResourceID id);
		VulkanDescriptorPool(ResourceID id, std::shared_ptr<VulkanContext> context, DescriptorType poolType, uint32_t descriptorCount, uint32_t maxSets);

	public:
		void Destroy();

	public:
		ResourceID AllocateDescriptorSets(ResourceID descriptorLayoutID, uint32_t count);

	public:
		VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkDescriptorPool m_DescriptorPool;
		DescriptorType m_DescriptorType;
	};
}