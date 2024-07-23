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
		Mesh(const std::filesystem::path& assetPath);

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

	public:
		void SetVertices(std::vector<VulkanVertex>& vertices);
		void SetIndices(std::vector<uint32_t>& indices);

	private: // Vertices
		std::string m_VerticesGUID;
		uint32_t m_VertexCount;
		std::vector<VulkanVertex> m_Vertices;
		ResourceHandle<VulkanVertexBuffer> m_VertexBuffer;

	private: // Indices
		std::string m_IndicesGUID;
		uint32_t m_IndexCount;
		std::vector<uint32_t> m_Indices;
		ResourceHandle<VulkanIndexBuffer> m_IndexBuffer;
	};
}