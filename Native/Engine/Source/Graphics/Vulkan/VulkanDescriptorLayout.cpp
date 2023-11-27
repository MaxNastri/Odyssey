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

	VulkanDescriptorLayout::VulkanDescriptorLayout(std::shared_ptr<VulkanContext> context, DescriptorType type, ShaderStage shaderStage, uint32_t bindingIndex)
	{
		m_Context = context;
		m_Type = type;
		m_ShaderStage = shaderStage;
		m_BindingIndex = bindingIndex;

		VkDescriptorSetLayoutBinding set_layout_binding{};
		set_layout_binding.descriptorType = ConvertDescriptorType(type);
		set_layout_binding.stageFlags = ConvertShaderFlags(shaderStage);
		set_layout_binding.binding = bindingIndex;
		set_layout_binding.descriptorCount = 1;

		VkDescriptorSetLayoutCreateInfo descriptor_layout_create_info{};
		descriptor_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_layout_create_info.bindingCount = 1;
		descriptor_layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
		descriptor_layout_create_info.pBindings = &set_layout_binding;

		if (vkCreateDescriptorSetLayout(m_Context->GetDeviceVK(), &descriptor_layout_create_info, nullptr, &m_Layout) != VK_SUCCESS)
		{
			Logger::LogError("(VulkanDescriptorLayout) Could not create descriptor set layout.");
		}
	}

	void VulkanDescriptorLayout::Destroy()
	{
		vkDestroyDescriptorSetLayout(m_Context->GetDeviceVK(), m_Layout, allocator);
	}
}