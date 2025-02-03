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

		assert(meshData.VertexLists.size() == meshData.IndexLists.size());
		m_SubMeshes.resize(meshData.VertexLists.size());

		for (size_t i = 0; i < m_SubMeshes.size(); i++)
		{
			if (meshData.VertexLists[i].size() > 0 &&
				meshData.IndexLists[i].size() > 0)
			{
				SetVertices(meshData.VertexLists[i], i);
				SetIndices(meshData.IndexLists[i], i);
			}
		}
	}

	void Mesh::SaveToDisk(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		serializer.WriteToDisk(path);
	}

	SubMesh* Mesh::GetSubmesh(size_t submeshIndex)
	{
		if (submeshIndex < m_SubMeshes.size())
			return &m_SubMeshes[submeshIndex];

		return nullptr;
	}

	void Mesh::SetVertices(const std::vector<Vertex>& vertices, size_t submeshIndex)
	{
		assert(submeshIndex < m_SubMeshes.size());

		SubMesh& submesh = m_SubMeshes[submeshIndex];
		submesh.Vertices = vertices;
		submesh.VertexCount = (uint16_t)submesh.Vertices.size();

		if (submesh.VertexBuffer)
			ResourceManager::Destroy(submesh.VertexBuffer);

		// Allocate the vertex buffer
		size_t dataSize = submesh.Vertices.size() * sizeof(submesh.Vertices[0]);
		submesh.VertexBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Vertex, dataSize);

		// Upload the vertices to the GPU
		Ref<VulkanBuffer> vertexBuffer = ResourceManager::GetResource<VulkanBuffer>(submesh.VertexBuffer);
		vertexBuffer->UploadData(vertices.data(), dataSize);
	}

	void Mesh::SetIndices(const std::vector<uint32_t>& indices, size_t submeshIndex)
	{
		assert(submeshIndex < m_SubMeshes.size());

		SubMesh& submesh = m_SubMeshes[submeshIndex];
		submesh.Indices = indices;
		submesh.IndexCount = (uint32_t)indices.size();

		if (submesh.IndexBuffer)
			ResourceManager::Destroy(submesh.IndexBuffer);

		size_t dataSize = submesh.Indices.size() * sizeof(submesh.Indices[0]);
		submesh.IndexBuffer = ResourceManager::Allocate<VulkanBuffer>(BufferType::Index, dataSize);
		Ref<VulkanBuffer> indexBuffer = ResourceManager::GetResource<VulkanBuffer>(submesh.IndexBuffer);
		indexBuffer->UploadData(indices.data(), dataSize);
	}
}