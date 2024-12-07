#pragma once
#include "Resource.h"
#include "Enums.h"

namespace Odyssey
{
	enum class Topology
	{
		LineList = 1,
		TriangleList = 2,
		TriangleStrip = 3,
	};

	enum class CullMode
	{
		None = 0,
		Back = 1,
		Front = 2,
	};

	struct VulkanPipelineInfo
	{
	public:
		std::map<ShaderType, ResourceID> Shaders;
		ResourceID DescriptorLayout;
		Topology Topology = Topology::TriangleList;
		CullMode CullMode = CullMode::None;
		bool FrontCCW = false;
		bool WriteDepth = true;
		bool BindVertexAttributeDescriptions = true;
		bool AlphaBlend = false;
		uint32_t MSAACountOverride = 0;
		bool IsShadow = false;
	};
}