#pragma once
#include "Resource.h"
#include "Enums.h"

namespace Odyssey
{
	struct VulkanPipelineInfo
	{
	public:
		std::map<ShaderType, ResourceID> Shaders;
		std::vector<ResourceID> DescriptorLayouts;
	};
}