#include "Mesh.h"
#include "ResourceManager.h"

namespace Odyssey
{
	Mesh::Mesh(const std::string& filename)
	{
		Deserialize(filename);

		m_VertexBuffer = ResourceManager::AllocateVertexBuffer(m_Vertices);
		m_IndexBuffer = ResourceManager::AllocateIndexBuffer(m_Indices);
	}

	void Mesh::Serialize(const std::string& filename)
	{
		// MeshData
		std::vector<char> meshData = SerializeMeshData();

		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;
		root["UUID"] << m_UUID;
		root["MeshData"] << std::string(meshData.begin(), meshData.end());

		FILE* file2 = fopen(filename.c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void Mesh::Deserialize(const std::string& filename)
	{
		if (std::ifstream ifs{ filename })
		{
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			std::string buffer;
			node["UUID"] >> m_UUID;
			node["MeshData"] >> buffer;

			std::vector<char> meshData(buffer.begin(), buffer.end());
			DeserializeMeshData(meshData);
		}
	}

	std::vector<char> Mesh::SerializeMeshData()
	{
		// Calculate the size of the data we are serializing
		size_t sizeOffset = sizeof(size_t);
		size_t vertexDataOffset = sizeof(VulkanVertex) * m_Vertices.size();
		size_t indexDataOffset = sizeof(uint32_t) * m_Indices.size();

		// Calculate the total size of buffer needed to store our typeless data
		size_t bufferSize = sizeOffset;
		bufferSize += vertexDataOffset;
		bufferSize += sizeOffset;
		bufferSize += indexDataOffset;

		size_t vertexCount = m_Vertices.size();
		size_t indexCount = m_Indices.size();

		std::vector<char> buffer(bufferSize);
		size_t index = 0;

		memcpy(&buffer[index], &vertexCount, sizeOffset);
		index += sizeOffset;

		memcpy(&buffer[index], m_Vertices.data(), vertexDataOffset);
		index += vertexDataOffset;

		memcpy(&buffer[index], &indexCount, sizeOffset);
		index += sizeOffset;

		memcpy(&buffer[index], m_Indices.data(), indexDataOffset);

		return buffer;
	}

	void Mesh::DeserializeMeshData(std::vector<char> buffer)
	{
		size_t sizeOffset = sizeof(size_t);

		m_Vertices.clear();
		m_Indices.clear();

		size_t index = 0;

		// Vertex data
		{

			// Read in the vertex count
			size_t vertexCount = 0;
			memcpy(&vertexCount, &buffer[index], sizeOffset);

			// Advance the buffer to the next element (vertex data)
			index += sizeOffset;

			// Resize the vertices to match the vertex count
			size_t vertexDataOffset = sizeof(VulkanVertex) * vertexCount;
			m_Vertices.resize(vertexCount);

			// Copy the vertex data into the vertices vector
			memcpy(m_Vertices.data(), &buffer[index], vertexDataOffset);

			// Advance the buffer to the next element (index count)
			index += vertexDataOffset;
		}

		// Index data
		{
			// Read in the index count
			size_t indexCount = 0;
			memcpy(&indexCount, &buffer[index], sizeOffset);

			// Advance the buffer to the next element (index data)
			index += sizeOffset;

			// Resize the indices to match the index count
			m_IndexCount = (uint32_t)indexCount;
			size_t indexDataOffset = sizeof(uint32_t) * indexCount;
			m_Indices.resize(indexCount);

			// Copy the index data into the indices vector
			memcpy(m_Indices.data(), &buffer[index], indexDataOffset);
		}

		m_VertexBuffer = ResourceManager::AllocateVertexBuffer(m_Vertices);
		m_IndexBuffer = ResourceManager::AllocateIndexBuffer(m_Indices);
	}
}