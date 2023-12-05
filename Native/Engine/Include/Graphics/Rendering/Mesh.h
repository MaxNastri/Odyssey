#pragma once
#include "Resource.h"
#include "Asset.h"
#include "ResourceHandle.h"
#include "VulkanVertex.h"
#include "ryml.hpp"

namespace Odyssey
{
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;

	class Mesh : public Resource, public Asset
	{
	public:
		Mesh(const std::string& filename); 

	public:
		void Serialize(const std::string& filename);
		void Deserialize(const std::string& filename);

	private:
		std::vector<char> SerializeMeshData();
		void DeserializeMeshData(std::vector<char> buffer);

	public:
		ResourceHandle<VulkanVertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		ResourceHandle<VulkanIndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		uint32_t GetIndexCount() { return m_IndexCount; }

	private:
		std::vector<VulkanVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_IndexCount;
		ResourceHandle<VulkanVertexBuffer> m_VertexBuffer;
		ResourceHandle<VulkanIndexBuffer> m_IndexBuffer;
	};
}