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
		VulkanDescriptorLayout(ResourceID id, std::shared_ptr<VulkanContext> context);
		void Destroy();

	public:
		void AddBinding(std::string_view bindingName, DescriptorType type, ShaderStage shaderStage, uint32_t bindingIndex);
		void Apply();

	public:
		VkDescriptorSetLayout GetHandle() { return m_Layout; }

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
		std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
	};
}