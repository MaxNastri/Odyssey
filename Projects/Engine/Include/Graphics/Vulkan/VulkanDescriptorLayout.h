#pragma once
#include "VulkanGlobals.h"
#include "Enums.h"
#include "Resource.h"

VK_FWD_DECLARE(VkDescriptorSetLayout)

namespace Odyssey
{
	class VulkanContext;

	class VulkanDescriptorLayout : public Resource
	{
	public:
		VulkanDescriptorLayout(std::shared_ptr<VulkanContext> context, DescriptorType type, ShaderStage shaderStage, uint32_t bindingIndex);
		void Destroy();

	public:
		VkDescriptorSetLayout GetHandle() { return m_Layout; }
		DescriptorType GetType() { return m_Type; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		DescriptorType m_Type;
		ShaderStage m_ShaderStage;
		uint32_t m_BindingIndex;
		VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
	};
}