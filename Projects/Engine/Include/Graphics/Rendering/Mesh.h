#pragma once
#include "Asset.h"
#include "Vertex.h"
#include "Resource.h"

namespace Odyssey
{
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;
	class SourceModel;

	class Mesh : public Asset
	{
		CLASS_DECLARATION(Odyssey, Mesh)
	public:
		Mesh() = default;
		Mesh(const Path& assetPath);
		Mesh(const Path& assetPath, std::shared_ptr<SourceModel> source);

	public:
		void Save();
		void Load();

	private:
		void LoadFromSource(std::shared_ptr<SourceModel> source);
		void SaveToDisk(const Path& assetPath);

	public:
		ResourceID GetVertexBuffer() { return m_VertexBuffer; }
		ResourceID GetIndexBuffer() { return m_IndexBuffer; }
		uint32_t GetIndexCount() { return m_IndexCount; }
		uint32_t GetVertexCount() { return m_VertexCount; }

	public:
		void SetVertices(std::vector<Vertex>& vertices);
		void SetIndices(std::vector<uint32_t>& indices);

	private: // Vertices
		GUID m_VerticesGUID = 0;
		uint32_t m_VertexCount;
		std::vector<Vertex> m_Vertices;
		ResourceID m_VertexBuffer;

	private: // Indices
		GUID m_IndicesGUID = 0;
		uint32_t m_IndexCount;
		std::vector<uint32_t> m_Indices;
		ResourceID m_IndexBuffer;
	};
}