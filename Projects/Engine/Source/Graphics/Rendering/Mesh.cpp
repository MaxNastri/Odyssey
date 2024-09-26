#include "Mesh.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
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
		LoadFromDisk(assetPath);

		m_VertexBuffer = ResourceManager::Allocate<VulkanVertexBuffer>(m_Vertices);
		m_IndexBuffer = ResourceManager::Allocate<VulkanIndexBuffer>(m_Indices);
	}

	Mesh::Mesh(const Path& assetPath, std::shared_ptr<SourceModel> source)
		: Asset(assetPath)
	{
		// FBX Import
		{
			// TODO: Add support for submeshes
			auto importer = source->GetImporter();
			const MeshImportData& meshData = importer->GetMeshData();
		
			// Transfer the mesh data from the importer
			m_VertexCount = (uint32_t)meshData.VertexLists[0].size();
			m_IndexCount = (uint32_t)meshData.IndexLists[0].size();
			m_Vertices = meshData.VertexLists[0];
			m_Indices = meshData.IndexLists[0];
		}

		// Create the binary asset for the vertices
		BinaryBuffer buffer;
		buffer.WriteData(m_Vertices);
		m_VerticesGUID = AssetManager::CreateBinaryAsset(buffer);

		// Create the binary asset for the indices
		buffer.Clear();
		buffer.WriteData(m_Indices);
		m_IndicesGUID = AssetManager::CreateBinaryAsset(buffer);

		// Allocate our buffers
		m_VertexBuffer = ResourceManager::Allocate<VulkanVertexBuffer>(m_Vertices);
		m_IndexBuffer = ResourceManager::Allocate<VulkanIndexBuffer>(m_Indices);
	}

	Mesh::Mesh(const Path& assetPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
		: Asset(assetPath)
	{
		m_Vertices = vertices;
		m_Indices = indices;
		m_VertexCount = m_Vertices.size();
		m_IndexCount = m_Indices.size();

		// Create the binary asset for the vertices
		BinaryBuffer buffer;
		buffer.WriteData(m_Vertices);
		m_VerticesGUID = AssetManager::CreateBinaryAsset(buffer);

		// Create the binary asset for the indices
		buffer.Clear();
		buffer.WriteData(m_Indices);
		m_IndicesGUID = AssetManager::CreateBinaryAsset(buffer);

		// Allocate our buffers
		m_VertexBuffer = ResourceManager::Allocate<VulkanVertexBuffer>(m_Vertices);
		m_IndexBuffer = ResourceManager::Allocate<VulkanIndexBuffer>(m_Indices);
	}

	void Mesh::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Mesh::Load()
	{
		LoadFromDisk(m_AssetPath);
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

	void Mesh::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			root.ReadData("m_VertexCount", m_VertexCount);
			root.ReadData("m_VertexData", m_VerticesGUID.Ref());
			root.ReadData("m_IndexCount", m_IndexCount);
			root.ReadData("m_IndexData", m_IndicesGUID.Ref());

			// Convert the vertex/index data from hex into real values
			if (m_VertexCount > 0 && m_VerticesGUID != 0)
			{
				BinaryBuffer buffer = AssetManager::LoadBinaryAsset(m_VerticesGUID);
				m_Vertices = buffer.Convert<Vertex>();
			}
			if (m_IndexCount > 0 && m_IndicesGUID != 0)
			{
				BinaryBuffer buffer = AssetManager::LoadBinaryAsset(m_IndicesGUID);
				m_Indices = buffer.Convert<uint32_t>();
			}
		}
	}

	void Mesh::SetVertices(std::vector<Vertex>& vertices)
	{
		m_Vertices = vertices;
		m_VertexCount = (uint16_t)m_Vertices.size();

		if (m_VertexBuffer)
			ResourceManager::Destroy(m_VertexBuffer);

		m_VertexBuffer = ResourceManager::Allocate<VulkanVertexBuffer>(m_Vertices);
	}

	void Mesh::SetIndices(std::vector<uint32_t>& indices)
	{
		m_Indices = indices;
		m_IndexCount = (uint32_t)m_Indices.size();

		if (m_IndexBuffer)
			ResourceManager::Destroy(m_IndexBuffer);

		m_IndexBuffer = ResourceManager::Allocate<VulkanIndexBuffer>(m_Indices);
	}
}