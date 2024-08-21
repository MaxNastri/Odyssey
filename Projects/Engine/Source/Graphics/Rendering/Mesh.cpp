#include "Mesh.h"
#include "ResourceManager.h"
#include "AssetSerializer.h"
#include "BinaryBuffer.h"
#include "AssetManager.h"

namespace Odyssey
{
	Mesh::Mesh(const std::filesystem::path& assetPath)
		: Asset(assetPath)
	{
		LoadFromDisk(assetPath);

		m_VertexBuffer = ResourceManager::AllocateVertexBuffer(m_Vertices);
		m_IndexBuffer = ResourceManager::AllocateIndexBuffer(m_Indices);
	}

	void Mesh::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Mesh::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	void Mesh::SaveToDisk(const std::filesystem::path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		root.WriteData("m_VertexCount", m_VertexCount);
		root.WriteData("m_VertexData",  m_VerticesGUID);
		root.WriteData("m_IndexCount", m_IndexCount);
		root.WriteData("m_IndexData", m_IndicesGUID);

		serializer.WriteToDisk(path);
	}

	struct OldVert
	{
	public:
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord0;
		glm::vec3 Color;
	};
	void Mesh::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			root.ReadData("m_VertexCount", m_VertexCount);
			root.ReadData("m_VertexData", m_VerticesGUID);
			root.ReadData("m_IndexCount", m_IndexCount);
			root.ReadData("m_IndexData", m_IndicesGUID);

			// Convert the vertex/index data from hex into real values
			if (m_VertexCount > 0 && m_VerticesGUID != "")
			{
				BinaryBuffer buffer = AssetManager::LoadBinaryAsset(m_VerticesGUID);
				m_Vertices = buffer.Convert<Vertex>();
			}
			if (m_IndexCount > 0 && m_IndicesGUID != "")
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

		if (m_VertexBuffer.IsValid())
			ResourceManager::DestroyVertexBuffer(m_VertexBuffer);

		m_VertexBuffer = ResourceManager::AllocateVertexBuffer(m_Vertices);
	}

	void Mesh::SetIndices(std::vector<uint32_t>& indices)
	{
		m_Indices = indices;
		m_IndexCount = (uint32_t)m_Indices.size();

		if (m_IndexBuffer.IsValid())
			ResourceManager::DestroyIndexBuffer(m_IndexBuffer);

		m_IndexBuffer = ResourceManager::AllocateIndexBuffer(m_Indices);
	}
}