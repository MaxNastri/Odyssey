#pragma once
#include "Resource.h"

namespace Odyssey
{
	struct VulkanPipelineInfo
	{
	public:
		ResourceID FragmentShader;
		ResourceID VertexShader;
		std::vector<ResourceID> DescriptorLayouts;
	};
}