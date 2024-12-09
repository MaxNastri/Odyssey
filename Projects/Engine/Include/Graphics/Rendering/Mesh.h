#pragma once
#include "Asset.h"
#include "Vertex.h"
#include "Resource.h"

namespace Odyssey
{
	class SourceModel;

	struct SubMesh
	{
	public:
		uint32_t VertexCount;
		uint32_t IndexCount;
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		ResourceID VertexBuffer;
		ResourceID IndexBuffer;
	};

	class Mesh : public Asset
	{
		CLASS_DECLARATION(Odyssey, Mesh)
	public:
		Mesh() = default;
		Mesh(const Path& assetPath);
		Mesh(const Path& assetPath, Ref<SourceModel> source);

	public:
		void Save();
		void Load();

	private:
		void LoadFromSource(Ref<SourceModel> source);
		void SaveToDisk(const Path& assetPath);

	public:
		ResourceID GetVertexBuffer(size_t submeshIndex = 0) { return m_SubMeshes[submeshIndex].VertexBuffer; }
		ResourceID GetIndexBuffer(size_t submeshIndex = 0) { return m_SubMeshes[submeshIndex].IndexBuffer; }
		uint32_t GetIndexCount(size_t submeshIndex = 0) { return m_SubMeshes[submeshIndex].IndexCount; }
		uint32_t GetVertexCount(size_t submeshIndex = 0) { return m_SubMeshes[submeshIndex].VertexCount; }
		SubMesh* GetSubmesh(size_t submeshIndex = 0);

	public:
		void SetVertices(const std::vector<Vertex>& vertices, size_t submeshIndex = 0);
		void SetIndices(const std::vector<uint32_t>& indices, size_t submeshIndex = 0);

	private:
		std::vector<SubMesh> m_SubMeshes;
	};
}