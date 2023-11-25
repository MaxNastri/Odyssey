#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "VulkanTypes.h"

VK_FWD_DECLARE(VkPipelineLayout)
VK_FWD_DECLARE(VkPipeline)

namespace Odyssey
{
	class VulkanContext;
	class VulkanShader;
	class VulkanDescriptorSet;

	class VulkanGraphicsPipeline : public Resource
	{
	public:
		VulkanGraphicsPipeline(std::shared_ptr<VulkanContext> context, VulkanPipelineInfo& info);
		void Destroy();

	public:
		VkPipeline GetPipeline() { return m_GraphicsPipeline; }

	private:
		void CreateLayout();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
	};
}