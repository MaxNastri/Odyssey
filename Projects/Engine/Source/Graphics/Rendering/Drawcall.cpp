#include "Drawcall.h"
#include "Mesh.h"

namespace Odyssey
{
	Drawcall::Drawcall(ResourceHandle<VulkanVertexBuffer> vertexBuffer, ResourceHandle<VulkanIndexBuffer> indexBuffer, uint32_t indexCount)
	{
		VertexBuffer = vertexBuffer;
		IndexBuffer = indexBuffer;
		indexCount = indexCount;
	}
}