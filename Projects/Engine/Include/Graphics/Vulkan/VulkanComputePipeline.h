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
		VulkanComputePipeline(std::shared_ptr<VulkanContext> context, VulkanPipelineInfo& info);
		void Destroy();

	private:
		std::shared_ptr<VulkanContext> m_Context;
		VkPipelineLayout m_PipelineLayout = nullptr;
		VkPipeline m_ComputePipeline = nullptr;
	};
}