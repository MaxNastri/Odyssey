#pragma once
#include "Resource.h"

namespace Odyssey
{
	class Mesh;
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;

	struct Drawcall
	{
	public:
		Drawcall() = default;
		Drawcall(ResourceID vertexBufferID, ResourceID indexBufferID, uint32_t indexCount);

	public:
		std::bitset<16> RenderLayers;
		ResourceID VertexBufferID;
		ResourceID IndexBufferID;
		uint32_t IndexCount;
		uint32_t UniformBufferIndex;
		uint32_t SkinningBufferIndex;
	};
}