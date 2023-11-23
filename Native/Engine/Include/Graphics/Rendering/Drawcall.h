#pragma once
#include "ResourceHandle.h"

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
		ResourceHandle<VulkanVertexBuffer> VertexBuffer;
		ResourceHandle<VulkanIndexBuffer> IndexBuffer;
		uint32_t IndexCount;
	};
}