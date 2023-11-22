#pragma once
#include "VulkanGlobals.h"

namespace Odyssey
{
	class VulkanContext;

	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(std::shared_ptr<VulkanContext> context);
		void Destroy();

	public:
		VkDescriptorSetLayout GetLayout() { return m_Layout; }
	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkDescriptorSetLayout m_Layout;

	};
}