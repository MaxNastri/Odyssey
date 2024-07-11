#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "Logger.h"

namespace Odyssey
{
	VulkanDescriptorSet::VulkanDescriptorSet(std::shared_ptr<VulkanContext> context, DescriptorType descriptorType, ResourceHandle<VulkanDescriptorPool> pool, std::vector<ResourceHandle<VulkanDescriptorLayout>> layouts, uint32_t count)
	{
		m_Context = context;
		m_Count = count;
		m_Type = descriptorType;

		// Create the allocate info struct
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool.Get()->GetDescriptorPool();
		allocInfo.descriptorSetCount = m_Count;

		// Convert the layout handles into a list of pointers
		std::vector<VkDescriptorSetLayout> setLayouts;
		for (ResourceHandle<VulkanDescriptorLayout> layout : layouts)
		{
			setLayouts.push_back(layout.Get()->GetHandle());
		}
		allocInfo.pSetLayouts = setLayouts.data();

		m_DescriptorSets.resize(m_Count);		
		if (vkAllocateDescriptorSets(m_Context->GetDeviceVK(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		{
			Logger::LogError("[VulkanDescriptorSet] Failed to allocate descriptor sets.");
		}
	}

	void VulkanDescriptorSet::SetBuffer(ResourceHandle<VulkanBuffer> buffer, uint32_t bindingIndex, uint32_t setIndex = 0)
	{
		VkDescriptorBufferInfo bufferInfo{ };
		bufferInfo.buffer = buffer.Get()->buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = static_cast<VkDeviceSize>(buffer.Get()->GetSize());

		VkWriteDescriptorSet writeInfo{ };
		writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet = m_DescriptorSets[setIndex];
		writeInfo.dstBinding = bindingIndex;
		writeInfo.dstArrayElement = setIndex;
		writeInfo.descriptorType = m_Type == DescriptorType::Uniform ?
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
			VK_DESCRIPTOR_TYPE_SAMPLER;
		writeInfo.descriptorCount = 1;
		writeInfo.pBufferInfo = &bufferInfo;
		writeInfo.pImageInfo = nullptr;
		writeInfo.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(m_Context->GetDeviceVK(), 1, &writeInfo, 0, nullptr);
	}
}