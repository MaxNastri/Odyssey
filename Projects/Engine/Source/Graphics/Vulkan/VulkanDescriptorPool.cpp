#include "VulkanDescriptorPool.h"
#include "VulkanContext.h"
#include "Logger.h"

namespace Odyssey
{
	VulkanDescriptorPool::VulkanDescriptorPool(std::shared_ptr<VulkanContext> context, DescriptorType poolType, uint32_t descriptorCount, uint32_t maxSets)
	{
		m_Context = context;
		m_DescriptorType = poolType;

		// TODO: Util function
		VkDescriptorType type = poolType == DescriptorType::Uniform ?
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_SAMPLER;

		VkDescriptorPoolSize poolSize{};
		poolSize.type = type;
		poolSize.descriptorCount = descriptorCount;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = maxSets;

		if (vkCreateDescriptorPool(m_Context->GetDeviceVK(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			Logger::LogError("[VulkanDescriptorPool] Failed to create descriptor pool.");
		}
	}

	void VulkanDescriptorPool::Destroy()
	{
		vkDestroyDescriptorPool(m_Context->GetDeviceVK(), m_DescriptorPool, nullptr);
	}

	ResourceHandle<VulkanDescriptorSet> VulkanDescriptorPool::AllocateDescriptorSets(ResourceHandle<VulkanDescriptorLayout> layout, uint32_t count)
	{
		return ResourceHandle<VulkanDescriptorSet>();
	}
}

