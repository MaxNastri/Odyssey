#include "Mesh.h"
#include "ResourceManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "Stopwatch.h"

namespace Odyssey
{
	Mesh::Mesh(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath)
		: Asset(assetPath, metaPath)
	{
		LoadFromDisk(assetPath);

		m_VertexBuffer = ResourceManager::AllocateVertexBuffer(m_Vertices);
		m_IndexBuffer = ResourceManager::AllocateIndexBuffer(m_Indices);
	}

	void Mesh::Save()
	{
		SaveToDisk(m_AssetPath);
		SaveMetadata();
	}

	void Mesh::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	void Mesh::SaveToDisk(const std::filesystem::path& path)
	{
		// Create a tree and root node
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		// Serialize the mesh-specific data
		root["m_VertexCount"] << m_VertexCount;
		root["m_VertexData"] << (m_VertexCount > 0 ? VertexDataToHex() : "");
		root["m_IndexCount"] << m_IndexCount;
		root["m_IndexData"] << (m_IndexCount > 0 ? IndexDataToHex() : "");

		// Save to disk
		FILE* file2 = fopen(path.string().c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void Mesh::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		if (std::ifstream ifs{ assetPath })
		{
			// Create the yaml root node
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			// Deserialize the mesh-specific data
			std::string vertexData;
			std::string indexData;
			node["m_VertexCount"] >> m_VertexCount;
			node["m_VertexData"] >> vertexData;
			node["m_IndexCount"] >> m_IndexCount;
			node["m_IndexData"] >> indexData;

			// Convert the vertex/index data from hex into real values
			if (m_VertexCount > 0 && vertexData != "")
				HexToVertexData(vertexData, m_VertexCount);
			if (m_IndexCount > 0 && indexData != "")
				HexToIndexData(indexData, m_IndexCount);
		}
	}

	std::string Mesh::VertexDataToHex()
	{
		// Start by converting the vertex data into binary
		size_t bufferSize = sizeof(VulkanVertex) * m_Vertices.size();
		std::vector<unsigned char> buffer(bufferSize);

		// Mem-copy the vertex data into the binary buffer
		memcpy(&buffer[0], m_Vertices.data(), bufferSize);

		std::stringstream stream;
		stream << std::hex << std::setfill('0');

		// Now lets convert the binary data into hex
		for (int i = 0; i < buffer.size(); i++)
		{
			stream << std::setw(sizeof(uint16_t)) << static_cast<uint16_t>(buffer[i]);
		}

		// Return the hex string
		return stream.str();
	}

	void Mesh::HexToVertexData(const std::string& hexData, uint32_t vertexCount)
	{
		// Read the hex data into a stream
		std::istringstream stream(hexData);
		stream >> std::hex;

		// Create a buffer for the binary data
		size_t bufferSize = sizeof(VulkanVertex) * vertexCount;
		std::vector<unsigned char> buffer(bufferSize);

		size_t index = 0;
		size_t stride = sizeof(uint16_t);
		for (uint32_t i = 0; i < bufferSize; i++)
		{
			std::string sub = hexData.substr(index, stride);
			buffer[i] = (unsigned char)strtol(sub.c_str(), NULL, 16);
			index += stride;
		}

		// Now resize the vertices to match the vertex count
		m_Vertices.resize(vertexCount);

		// Copy the binary vertex data into the vertices vector
		memcpy(m_Vertices.data(), &buffer[0], bufferSize);
	}

	std::string Mesh::IndexDataToHex()
	{
		// Start by converting the vertex data into binary
		size_t bufferSize = sizeof(uint32_t) * m_Indices.size();
		std::vector<unsigned char> buffer(bufferSize);

		// Mem-copy the vertex data into the binary buffer
		memcpy(&buffer[0], m_Indices.data(), bufferSize);

		std::stringstream stream;
		stream << std::hex << std::setfill('0');

		// Now lets convert the binary data into hex
		for (int i = 0; i < buffer.size(); i++)
		{
			stream << std::setw(sizeof(uint16_t)) << static_cast<uint16_t>(buffer[i]);
		}

		// Return the hex string
		return stream.str();
	}

	void Mesh::HexToIndexData(const std::string& hexData, uint32_t indexCount)
	{
		// Read the hex data into a stream
		std::istringstream stream(hexData);
		stream >> std::hex;

		// Create a buffer for the binary data
		size_t bufferSize = sizeof(uint32_t) * indexCount;
		std::vector<unsigned char> buffer(bufferSize);

		size_t index = 0;
		size_t stride = sizeof(uint16_t);
		for (uint32_t i = 0; i < bufferSize; i++)
		{
			std::string sub = hexData.substr(index, stride);
			buffer[i] = (unsigned char)strtol(sub.c_str(), NULL, 16);
			index += stride;
		}

		// Now resize the vertices to match the vertex count
		m_Indices.resize(indexCount);

		// Copy the binary vertex data into the vertices vector
		memcpy(m_Indices.data(), &buffer[0], bufferSize);
	}

	void Mesh::SetVertices(std::vector<VulkanVertex>& vertices)
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