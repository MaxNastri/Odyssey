#include "VulkanGraphicsPipeline.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanShaderModule.h"
#include "Vertex.h"
#include "VulkanDescriptorLayout.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Material.h"

namespace Odyssey
{

	VkPrimitiveTopology ConvertTopology(Topology topology)
	{
		switch (topology)
		{
			case Topology::LineList:
				return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case Topology::TriangleList:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case Topology::TriangleStrip:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case Topology::PatchList:
				return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			default:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	VkFormat GetVkFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::None:
			case TextureFormat::R8G8B8_UNORM:
				return VK_FORMAT_R8G8B8_SRGB;
			case TextureFormat::R8G8B8A8_SRGB:
				return VK_FORMAT_R8G8B8A8_SRGB;
			case TextureFormat::R8G8B8A8_UNORM:
				return VK_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::R16G16B16_SFLOAT:
				return VK_FORMAT_R16G16B16_SFLOAT;
			case TextureFormat::R16G16B16A16_SFLOAT:
				return VK_FORMAT_R16G16B16A16_SFLOAT;
			case TextureFormat::R32G32B32A32_SFLOAT:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			case TextureFormat::R16G16_SFLOAT:
				return VK_FORMAT_R16G16_SFLOAT;
			case TextureFormat::D24_UNORM_S8_UINT:
				return VK_FORMAT_D24_UNORM_S8_UINT;
			case TextureFormat::D16_UNORM:
				return VK_FORMAT_D16_UNORM;
			case TextureFormat::D32_SFLOAT:
				return VK_FORMAT_D32_SFLOAT;
			case TextureFormat::D32_SFLOAT_S8_UINT:
				return VK_FORMAT_D32_SFLOAT_S8_UINT;
			default:
				return VK_FORMAT_R8G8B8A8_UNORM;
		}
	}

	VkCullModeFlags ConvertCullMode(CullMode cullMode)
	{
		switch (cullMode)
		{
			case CullMode::None:
				return VK_CULL_MODE_NONE;
			case CullMode::Back:
				return VK_CULL_MODE_BACK_BIT;
			case CullMode::Front:
				return VK_CULL_MODE_FRONT_BIT;
			default:
				return VK_CULL_MODE_NONE;
		}
	}

	VulkanGraphicsPipeline::VulkanGraphicsPipeline(ResourceID id, std::shared_ptr<VulkanContext> context, VulkanPipelineInfo& info)
		: Resource(id)
	{
		m_Context = context;

		CreateLayout(info);

		// Shaders
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		for (auto [shaderType, ResourceID] : info.Shaders)
		{
			// Skip compute shaders for graphics pipelines
			if (shaderType == ShaderType::Compute)
				continue;

			if (shaderType == ShaderType::Geometry)
			{
				info.CullMode = CullMode::None;
			}
			if (shaderType == ShaderType::Hull || shaderType == ShaderType::Domain)
				info.Topology = Topology::PatchList;

			auto shader = ResourceManager::GetResource<VulkanShaderModule>(ResourceID);

			VkPipelineShaderStageCreateInfo shaderStageInfo{};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = shader->GetShaderFlags();
			shaderStageInfo.module = shader->GetShaderModule();
			shaderStageInfo.pName = "main";
			shaderStages.push_back(shaderStageInfo);
		}

		// Dynamics
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_DEPTH_BIAS
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		// Viewport - Note: We are using the simplified version because we are using dynamic states above
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Vertex input
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		if (info.AttributeDescriptions.GetSize() > 0)
		{
			VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

			auto attributeDescriptions = info.AttributeDescriptions.Convert<VkVertexInputAttributeDescription>();
			vertexInputInfo.vertexAttributeDescriptionCount = info.AttributeDescriptions.GetCount();
			vertexInputInfo.pVertexAttributeDescriptions = (VkVertexInputAttributeDescription*)info.AttributeDescriptions.GetData().data();
		}

		// Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = ConvertTopology(info.Topology);
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		// TODO: Convert this to be dynamically set based on config/per material
		rasterizer.cullMode = ConvertCullMode(info.CullMode);
		rasterizer.frontFace = info.FrontCCW ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = info.IsShadow ? VK_TRUE : VK_FALSE;

		// MSAA
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;

		if (info.MSAACountOverride)
			multisampling.rasterizationSamples = (VkSampleCountFlagBits)info.MSAACountOverride;
		else if (m_Context->GetSampleCount() > 1)
			multisampling.rasterizationSamples = (VkSampleCountFlagBits)m_Context->GetSampleCount();
		else
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = info.TestDepth;
		depthStencil.depthWriteEnable = info.WriteDepth;

		if (info.IsShadow)
			depthStencil.depthCompareOp = Renderer::ReverseDepthEnabled() ? VK_COMPARE_OP_GREATER_OR_EQUAL : VK_COMPARE_OP_LESS_OR_EQUAL;
		else
			depthStencil.depthCompareOp = Renderer::ReverseDepthEnabled() ? VK_COMPARE_OP_GREATER : VK_COMPARE_OP_LESS;

		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		// Normal color blending
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};

		if (info.SetBlendMode == BlendMode::AlphaBlend)
		{
			// Alpha blend
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}
		else if (info.SetBlendMode == BlendMode::Additive)
		{
			// Additive blend
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}
		else
		{
			// No blending
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;
		}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 1.0f; // Optional
		colorBlending.blendConstants[1] = 1.0f; // Optional
		colorBlending.blendConstants[2] = 1.0f; // Optional
		colorBlending.blendConstants[3] = 1.0f; // Optional

		VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info{};
		pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;

		if (info.ColorFormat == TextureFormat::None)
		{
			pipeline_rendering_create_info.colorAttachmentCount = 0;
		}
		else
		{
			pipeline_rendering_create_info.colorAttachmentCount = 1;
			VkFormat colorAttachmentFormat = GetVkFormat(info.ColorFormat);
			assert(colorAttachmentFormat != VK_FORMAT_UNDEFINED);
			pipeline_rendering_create_info.pColorAttachmentFormats = &colorAttachmentFormat;
		}
		if (info.DepthFormat != TextureFormat::None)
			pipeline_rendering_create_info.depthAttachmentFormat = GetVkFormat(info.DepthFormat);

		if (info.DepthFormat == TextureFormat::D24_UNORM_S8_UINT)
			pipeline_rendering_create_info.stencilAttachmentFormat = GetVkFormat(info.DepthFormat);

		VkPipelineTessellationStateCreateInfo tesselationInfo = {};
		tesselationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		tesselationInfo.patchControlPoints = 3;
		tesselationInfo.pNext = nullptr;
		tesselationInfo.flags = 0;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = (uint32_t)shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.pTessellationState = &tesselationInfo;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = nullptr;
		pipelineInfo.pNext = &pipeline_rendering_create_info;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(m_Context->GetDevice()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
		{
			Log::Error("[VulkanGraphicsPipeline] Failed to create graphics pipeline.");
			return;
		}
	}

	void VulkanGraphicsPipeline::Destroy()
	{
		vkDestroyPipelineLayout(m_Context->GetDevice()->GetLogicalDevice(), m_PipelineLayout, allocator);
		vkDestroyPipeline(m_Context->GetDevice()->GetLogicalDevice(), m_GraphicsPipeline, nullptr);
	}

	void VulkanGraphicsPipeline::CreateLayout(VulkanPipelineInfo& info)
	{
		// Convert resource handles to vulkan data
		std::vector<VkDescriptorSetLayout> setLayouts{};

		if (auto layout = ResourceManager::GetResource<VulkanDescriptorLayout>(info.DescriptorLayout))
			setLayouts.push_back(layout->GetHandle());

		std::vector<VkPushConstantRange> pushConstants;

		for (PushConstantRange pushConstant : info.PushConstantRanges)
		{
			VkPushConstantRange& vkRange = pushConstants.emplace_back();
			vkRange.stageFlags = pushConstant.Flags;
			vkRange.offset = pushConstant.Offset;
			vkRange.size = pushConstant.Size;
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)setLayouts.size(); // Optional
		pipelineLayoutInfo.pSetLayouts = setLayouts.data(); // Optional
		pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)pushConstants.size(); // Optional
		pipelineLayoutInfo.pPushConstantRanges = pushConstants.data(); // Optional

		if (vkCreatePipelineLayout(m_Context->GetDevice()->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			Log::Error("[VulkanGraphicsPipeline] Failed to create pipeline layout.");
			return;
		}
	}
}