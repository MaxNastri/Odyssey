#pragma once
#include "Resource.h"
#include "Enums.h"
#include "BinaryBuffer.h"

namespace Odyssey
{
	enum class Topology
	{
		LineList = 1,
		TriangleList = 2,
		TriangleStrip = 3,
		PatchList = 4,
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
		BlendMode SetBlendMode = BlendMode::None;
		TextureFormat ColorFormat = TextureFormat::R8G8B8A8_UNORM;
		TextureFormat DepthFormat = TextureFormat::D24_UNORM_S8_UINT;
		uint32_t MSAACountOverride = 0;
		bool IsShadow = false;
		BinaryBuffer AttributeDescriptions;
	};
}