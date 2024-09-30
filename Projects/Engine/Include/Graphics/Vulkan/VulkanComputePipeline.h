#pragma once
#include "Resource.h"
#include "VulkanGlobals.h"
#include "VulkanTypes.h"

namespace Odyssey
{
	class VulkanContext;

	class VulkanComputePipeline : public Resource
	{
	public:
		VulkanComputePipeline(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanPipelineInfo& info);
		void Destroy();

	public:
		VkPipeline GetPipeline() { return m_ComputePipeline; }
		VkPipelineLayout GetLayout() { return m_PipelineLayout; }

	private:
		void CreateLayout(VulkanPipelineInfo& info);

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkPipelineLayout m_PipelineLayout = nullptr;
		VkPipeline m_ComputePipeline = nullptr;
	};
}