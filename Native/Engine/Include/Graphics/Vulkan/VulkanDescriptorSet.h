#pragma once
#include "Enums.h"
#include "VulkanGlobals.h"
#include "ResourceHandle.h"
#include "Resource.h"

VK_FWD_DECLARE(VkDescriptorSet)

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanContext;
	class VulkanDescriptorPool;
	class VulkanDescriptorLayout;

	class VulkanDescriptorSet : public Resource
	{
	public:
		VulkanDescriptorSet(std::shared_ptr<VulkanContext> context, DescriptorType descriptorType, ResourceHandle<VulkanDescriptorPool> pool, std::vector<ResourceHandle<VulkanDescriptorLayout>> layouts, uint32_t count);

	public:
		void SetBuffer(ResourceHandle<VulkanBuffer> buffer, uint32_t bindingIndex, uint32_t arrayElement = 0);
		void SetImage();
		std::vector<VkDescriptorSet> GetDescriptorSets() { return m_DescriptorSets; }
		uint32_t GetCount() { return m_Count; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::vector<VkDescriptorSet> m_DescriptorSets;
		uint32_t m_Count;
		DescriptorType m_Type;
	};
}