#pragma once
#include "Asset.h"
#include "AssetHandle.h"
#include "ResourceHandle.h"
#include "Vertex.h"
#include "GUID.h"

namespace Odyssey
{
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;
	class SourceModel;

	class Mesh : public Asset
	{
	public:
		Mesh() = default;
		Mesh(const Path& assetPath);
		Mesh(const Path& assetPath, AssetHandle<SourceModel> source);

	public:
		void Save();
		void Load();

	private:
		void SaveToDisk(const std::filesystem::path& assetPath);
		void LoadFromDisk(const std::filesystem::path& assetPath);

	public:
		ResourceHandle<VulkanVertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		ResourceHandle<VulkanIndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		uint32_t GetIndexCount() { return m_IndexCount; }
		uint32_t GetVertexCount() { return m_VertexCount; }

	public:
		void SetVertices(std::vector<Vertex>& vertices);
		void SetIndices(std::vector<uint32_t>& indices);

	private: // Vertices
		GUID m_VerticesGUID = 0;
		uint32_t m_VertexCount;
		std::vector<Vertex> m_Vertices;
		ResourceHandle<VulkanVertexBuffer> m_VertexBuffer;

	private: // Indices
		GUID m_IndicesGUID = 0;
		uint32_t m_IndexCount;
		std::vector<uint32_t> m_Indices;
		ResourceHandle<VulkanIndexBuffer> m_IndexBuffer;
	};
}