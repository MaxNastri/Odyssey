#pragma once
#include "Enums.h"
#include "VulkanGlobals.h"
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
		VulkanDescriptorSet(ResourceID id, std::shared_ptr<VulkanContext> context, DescriptorType descriptorType, ResourceID descriptorPoolID, ResourceID descriptorLayoutID, uint32_t count);

	public:
		void SetBuffer(ResourceID bufferID, uint32_t bindingIndex, uint32_t arrayElement = 0);
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