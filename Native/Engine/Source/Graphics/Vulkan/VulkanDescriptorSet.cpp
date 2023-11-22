#include "VulkanDescriptorSet.h"
#include "VulkanContext.h"

namespace Odyssey
{
	VulkanDescriptorSet::VulkanDescriptorSet(std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(m_Context->GetDeviceVK(), &layoutInfo, nullptr, &m_Layout) != VK_SUCCESS)
		{
			Logger::LogError("(VulkanDescriptorSet) Failed to create descriptor set layout.");
			return;
		}
	}
	void VulkanDescriptorSet::Destroy()
	{
		vkDestroyDescriptorSetLayout(m_Context->GetDeviceVK(), m_Layout, allocator);
	}
}