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
		Drawcall() = default;
		Drawcall(ResourceHandle<VulkanVertexBuffer> vertexBuffer, ResourceHandle<VulkanIndexBuffer> indexBuffer, uint32_t indexCount);

	public:
		std::bitset<16> RenderLayers;
		ResourceHandle<VulkanVertexBuffer> VertexBuffer;
		ResourceHandle<VulkanIndexBuffer> IndexBuffer;
		uint32_t IndexCount;
		uint32_t UniformBufferIndex;
	};
}