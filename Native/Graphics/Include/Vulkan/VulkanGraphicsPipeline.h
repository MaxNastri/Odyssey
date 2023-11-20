#pragma once
#include "VulkanGlobals.h"

VK_FWD_DECLARE(VkPipelineLayout)
VK_FWD_DECLARE(VkPipeline)

namespace Odyssey
{
	class VulkanContext;
	class VulkanShader;

	struct VulkanPipelineInfo
	{
		VulkanShader* fragmentShader;
		VulkanShader* vertexShader;
	};


	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(std::shared_ptr<VulkanContext> context, const VulkanPipelineInfo& info);
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