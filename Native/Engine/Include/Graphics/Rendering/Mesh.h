#pragma once
#include "Asset.h"
#include "ResourceHandle.h"
#include "VulkanVertex.h"
#include "ryml.hpp"

namespace Odyssey
{
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;

	class Mesh : public Asset
	{
	public:
		Mesh() = default;
		Mesh(const std::string& filename);

	public:
		void Save(const std::string& filename);
		void Load(const std::string& filename);

	private:
		std::string VertexDataToHex();
		void HexToVertexData(const std::string& hexData, uint32_t vertexCount);
		std::string IndexDataToHex();
		void HexToIndexData(const std::string& hexData, uint32_t indexCount);

	public:
		ResourceHandle<VulkanVertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		ResourceHandle<VulkanIndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		uint32_t GetIndexCount() { return m_IndexCount; }

	public:
		void SetVertices(std::vector<VulkanVertex>& vertices);
		void SetIndices(std::vector<uint32_t>& indices);

	private:
		std::vector<VulkanVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_VertexCount;
		uint32_t m_IndexCount;
		ResourceHandle<VulkanVertexBuffer> m_VertexBuffer;
		ResourceHandle<VulkanIndexBuffer> m_IndexBuffer;
	};
}