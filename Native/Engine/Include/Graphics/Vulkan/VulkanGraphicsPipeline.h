#pragma once
#include "VulkanGlobals.h"
#include "VulkanTypes.h"

VK_FWD_DECLARE(VkPipelineLayout)
VK_FWD_DECLARE(VkPipeline)

namespace Odyssey
{
	class VulkanContext;
	class VulkanShader;
	class VulkanDescriptorSet;

	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(std::shared_ptr<VulkanContext> context, const VulkanPipelineInfo& info);
		void Destroy();

	public:
		void AddDescriptorSet(std::shared_ptr<VulkanDescriptorSet> descriptorSet);

	public:
		VkPipeline GetPipeline() { return m_GraphicsPipeline; }

	private:
		void CreateLayout();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		std::vector<std::shared_ptr<VulkanDescriptorSet>> m_DescriptorSets;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
	};
}