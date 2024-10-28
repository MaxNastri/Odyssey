#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "VulkanTypes.h"

VK_FWD_DECLARE(VkPipelineLayout)
VK_FWD_DECLARE(VkPipeline)

namespace Odyssey
{
	class VulkanContext;
	class VulkanShaderModule;
	class VulkanDescriptorSet;

	class VulkanGraphicsPipeline : public Resource
	{
	public:
		VulkanGraphicsPipeline(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanPipelineInfo& info);
		void Destroy();

	public:
		VkPipeline GetPipeline() { return m_GraphicsPipeline; }
		VkPipelineLayout GetLayout() { return m_PipelineLayout; }

	private:
		void CreateLayout(VulkanPipelineInfo& info);
	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
	};
}