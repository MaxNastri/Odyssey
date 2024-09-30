#pragma once
#include "Resource.h"
#include "Enums.h"

namespace Odyssey
{
	struct VulkanPipelineInfo
	{
	public:
		std::map<ShaderType, ResourceID> Shaders;
		ResourceID DescriptorLayout;
		bool Triangles = true;
		bool Strips = false;
		bool WriteDepth = true;
		bool UseParticle = false;
	};
}