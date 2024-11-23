#include "Mesh.h"
#include "VulkanBuffer.h"
#include "ResourceManager.h"
#include "AssetSerializer.h"
#include "BinaryBuffer.h"
#include "AssetManager.h"
#include "SourceModel.h"

namespace Odyssey
{
	Mesh::Mesh(const Path& assetPath)
		: Asset(assetPath)
	{
		if (Ref<SourceModel> source = AssetManager::LoadSourceAsset<SourceModel>(m_SourceAsset))
			LoadFromSource(source);
	}

	Mesh::Mesh(const Path& assetPath, Ref<SourceModel> source)
		: Asset(assetPath)
	{
		SetSourceAsset(source->GetGUID());
		LoadFromSource(source);
	}

	void Mesh::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Mesh::Load()
	{
		if (Ref<SourceModel> source = AssetManager::LoadSourceAsset<SourceModel>(m_SourceAsset))
			LoadFromSource(source);
	}

	void Mesh::LoadFromSource(Ref<SourceModel> source)
	{
		// TODO: Add support for submeshes
		auto importer = source->GetImporter();
		const MeshImportData& meshData = importer->GetMeshData();

		std::vector<Vertex> vertices = meshData.VertexLists[0];
		SetVertices(vertices);

		std::vector<uint32_t> indices = meshData.IndexLists[0];
		SetIndices(indices);
	}

	void Mesh::SaveToDisk(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		root.WriteData("m_VertexCount", m_VertexCount);
		root.WriteData("m_VertexData", m_VerticesGUID.CRef());
		root.WriteData("m_IndexCount", m_IndexCount);
		root.WriteData("m_IndexData", m_IndicesGUID.CRef());

		serializer.WriteToDisk(path);
	}

	void Mesh::SetVertices(std::vector<Vertex>& vertices)
	{
		m_Vertices = vertices;
		m_VertexCount = (uint16_t)m_Vertices.size();

		if (m_VertexBuffer)
			ResourceManager::Destroy(m_VertexBuffer);

		// Allocate the vertex buffer
		size_t dataSize = m_Vertices.size() * sizeof(m_Vertices[0]);
		m_VertexBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Vertex, dataSize);

		// Upload the vertices to the GPU
		auto vertexBuffer = ResourceManager::GetResource<VulkanBuffer>(m_VertexBuffer);
		vertexBuffer->UploadData(vertices.data(), dataSize);
	}

	void Mesh::SetIndices(std::vector<uint32_t>& indices)
	{
		m_Indices = indices;
		m_IndexCount = (uint32_t)m_Indices.size();

		if (m_IndexBuffer)
			ResourceManager::Destroy(m_IndexBuffer);

		size_t dataSize = m_Indices.size() * sizeof(m_Indices[0]);
		m_IndexBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Index, dataSize);
		auto indexBuffer = ResourceManager::GetResource<VulkanBuffer>(m_IndexBuffer);
		indexBuffer->UploadData(indices.data(), dataSize);
	}
}