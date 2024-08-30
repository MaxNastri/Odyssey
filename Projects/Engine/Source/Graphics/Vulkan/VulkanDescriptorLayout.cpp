#include "VulkanDescriptorLayout.h"
#include "VulkanContext.h"
#include "volk.h"
#include "Logger.h"

namespace Odyssey
{
	VkDescriptorType ConvertDescriptorType(DescriptorType type)
	{
		switch (type)
		{
			case Odyssey::None:
				Logger::LogError("(VulkanDescriptorLayout) Cannot convert descriptor type None");
				return (VkDescriptorType)0;
			case Odyssey::Uniform:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case Odyssey::Sampler:
				return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			default:
				Logger::LogError("(VulkanDescriptorLayout) Cannot convert descriptor type None");
				return (VkDescriptorType)0;

		}
	}

	VkShaderStageFlags ConvertShaderFlags(ShaderStage shaderStage)
	{
		switch (shaderStage)
		{
			case Odyssey::ShaderStage::None:
				Logger::LogError("(VulkanDescriptorLayout) Cannot convert shader stage None");
				return (VkDescriptorType)0;
			case Odyssey::ShaderStage::Fragment:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			case Odyssey::ShaderStage::Vertex:
				return VK_SHADER_STAGE_VERTEX_BIT;
			default:
				Logger::LogError("(VulkanDescriptorLayout) Cannot convert shader stage None");
				return (VkDescriptorType)0;
		}
	}

	VulkanDescriptorLayout::VulkanDescriptorLayout(std::shared_ptr<VulkanContext> context)
	{
		m_Context = context;
	}

	void VulkanDescriptorLayout::Destroy()
	{
		vkDestroyDescriptorSetLayout(m_Context->GetDeviceVK(), m_Layout, allocator);
	}

	void VulkanDescriptorLayout::AddBinding(std::string_view bindingName, DescriptorType type, ShaderStage shaderStage, uint32_t bindingIndex)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.descriptorType = ConvertDescriptorType(type);
		layoutBinding.stageFlags = ConvertShaderFlags(shaderStage);
		layoutBinding.binding = bindingIndex;
		layoutBinding.descriptorCount = 1;
		m_Bindings.push_back(layoutBinding);
	}

	void VulkanDescriptorLayout::Apply()
	{
		VkDescriptorSetLayoutCreateInfo descriptor_layout_create_info{};
		descriptor_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
		descriptor_layout_create_info.bindingCount = (uint32_t)(m_Bindings.size());
		descriptor_layout_create_info.pBindings = m_Bindings.data();

		if (vkCreateDescriptorSetLayout(m_Context->GetDeviceVK(), &descriptor_layout_create_info, nullptr, &m_Layout) != VK_SUCCESS)
		{
			Logger::LogError("(VulkanDescriptorLayout) Could not create descriptor set layout.");
		}
	}
}