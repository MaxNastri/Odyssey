#pragma once
#include "ResourceHandle.h"

namespace Odyssey
{
	class VulkanShader;
	class VulkanDescriptorLayout;

	struct VulkanPipelineInfo
	{
	public:
		ResourceHandle<VulkanShader> fragmentShader;
		ResourceHandle<VulkanShader> vertexShader;
		std::vector<ResourceHandle<VulkanDescriptorLayout>> descriptorLayouts;
	};
}