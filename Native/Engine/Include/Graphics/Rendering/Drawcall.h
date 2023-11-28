#pragma once
#include "ResourceHandle.h"
#include <bitset>
#include "glm.h"

namespace Odyssey
{
	class Mesh;
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;

	struct Drawcall
	{
	public:
		void SetMesh(ResourceHandle<Mesh> mesh);

	public:
		std::bitset<16> RenderLayers;
		ResourceHandle<VulkanVertexBuffer> VertexBuffer;
		ResourceHandle<VulkanIndexBuffer> IndexBuffer;
		uint32_t IndexCount;
		uint32_t UniformBufferIndex;
	};
}