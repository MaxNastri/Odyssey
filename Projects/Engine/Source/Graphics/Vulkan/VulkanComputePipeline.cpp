#include "VulkanComputePipeline.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "ResourceManager.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanShaderModule.h"

namespace Odyssey
{
	VulkanComputePipeline::VulkanComputePipeline(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanPipelineInfo& info)
		: Resource(id)
	{
		m_Context = context;
		CreateLayout(info);

		// Shaders
		VkPipelineShaderStageCreateInfo shaderStage{};

		assert(info.Shaders.contains(ShaderType::Compute));

		auto shader = ResourceManager::GetResource<VulkanShaderModule>(info.Shaders[ShaderType::Compute]);

		shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage.stage = shader->GetShaderFlags();
		shaderStage.module = shader->GetShaderModule();
		shaderStage.pName = "main";

		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.stage = shaderStage;

		if (vkCreateComputePipelines(m_Context->GetDevice()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_ComputePipeline) != VK_SUCCESS)
		{
			Log::Error("[VulkanComputePipeline] Failed to create graphics pipeline.");
			return;
		}
	}

	void VulkanComputePipeline::Destroy()
	{
		vkDestroyPipelineLayout(m_Context->GetDevice()->GetLogicalDevice(), m_PipelineLayout, allocator);
		vkDestroyPipeline(m_Context->GetDevice()->GetLogicalDevice(), m_ComputePipeline, nullptr);
	}

	void VulkanComputePipeline::CreateLayout(VulkanPipelineInfo& info)
	{
		// Convert resource handles to vulkan data
		std::vector<VkDescriptorSetLayout> setLayouts{};

		if (auto layout = ResourceManager::GetResource<VulkanDescriptorLayout>(info.DescriptorLayout))
			setLayouts.push_back(layout->GetHandle());

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)setLayouts.size(); // Optional
		pipelineLayoutInfo.pSetLayouts = setLayouts.data(); // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(m_Context->GetDevice()->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			Log::Error("[VulkanGraphicsPipeline] Failed to create pipeline layout.");
			return;
		}
	}
}