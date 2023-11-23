#include "Mesh.h"
#include "ResourceManager.h"

namespace Odyssey
{
	Mesh::Mesh(std::vector<VulkanVertex>& vertices, std::vector<uint32_t>& indices)
	{
		m_Vertices = vertices;
		m_Indices = indices;
		m_IndexCount = (uint32_t)m_Indices.size();
		m_VertexBuffer = ResourceManager::AllocateVertexBuffer(m_Vertices);
		m_IndexBuffer = ResourceManager::AllocateIndexBuffer(m_Indices);
	}

	Mesh::Mesh(ResourceHandle<VulkanVertexBuffer> vertexBuffer, ResourceHandle<VulkanIndexBuffer> indexBuffer)
	{
		// TODO (MAX): Map the vertices from the buffers into our arrays
		m_Vertices.clear();
		m_Indices.clear();
		m_IndexCount = 0;

		m_VertexBuffer = vertexBuffer;
		m_IndexBuffer = indexBuffer;
	}
}