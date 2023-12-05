#pragma once
#include "ResourceHandle.h"

namespace Odyssey
{
	class VulkanShaderModule;
	class VulkanDescriptorLayout;

	struct VulkanPipelineInfo
	{
	public:
		ResourceHandle<VulkanShaderModule> fragmentShader;
		ResourceHandle<VulkanShaderModule> vertexShader;
		std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts;
	};
}