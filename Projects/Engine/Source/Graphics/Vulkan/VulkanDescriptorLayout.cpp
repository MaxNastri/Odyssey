#include "VulkanDescriptorLayout.h"
#include "VulkanContext.h"
#include "volk.h"
#include "Log.h"

namespace Odyssey
{
	VkDescriptorType ConvertDescriptorType(DescriptorType type)
	{
		switch (type)
		{
			case DescriptorType::None:
				Log::Error("(VulkanDescriptorLayout) Cannot convert descriptor type None");
				return (VkDescriptorType)0;
			case DescriptorType::Uniform:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case DescriptorType::Sampler:
				return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case DescriptorType::Storage:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			default:
				Log::Error("(VulkanDescriptorLayout) Cannot convert descriptor type None");
				return (VkDescriptorType)0;

		}
	}

	VkShaderStageFlags ConvertShaderFlags(ShaderStage shaderStage)
	{
		switch (shaderStage)
		{
			case ShaderStage::None:
				Log::Error("(VulkanDescriptorLayout) Cannot convert shader stage None");
				return (VkDescriptorType)0;
			case ShaderStage::Fragment:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			case ShaderStage::Vertex:
				return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderStage::Compute:
				return VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderStage::Geometry:
				return VK_SHADER_STAGE_GEOMETRY_BIT;
			default:
				Log::Error("(VulkanDescriptorLayout) Cannot convert shader stage None");
				return (VkDescriptorType)0;
		}
	}

	VulkanDescriptorLayout::VulkanDescriptorLayout(ResourceID id, std::shared_ptr<VulkanContext> context)
		: Resource(id)
	{
		m_Context = context;
	}

	void VulkanDescriptorLayout::Destroy()
	{
		vkDestroyDescriptorSetLayout(m_Context->GetDeviceVK(), m_Layout, allocator);
	}

	void VulkanDescriptorLayout::AddBinding(std::string_view bindingName, DescriptorType type, uint32_t bindingIndex)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.descriptorType = ConvertDescriptorType(type);
		// Note: We can set Shader_Stage_All because we are using push descriptors
		// By default, push descriptors are shared across all shader stages
		layoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
		layoutBinding.binding = (uint32_t)bindingIndex;
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
			Log::Error("(VulkanDescriptorLayout) Could not create descriptor set layout.");
		}
	}
}