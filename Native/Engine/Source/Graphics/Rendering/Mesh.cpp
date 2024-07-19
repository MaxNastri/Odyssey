#include "Mesh.h"
#include "ResourceManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "Stopwatch.h"
#include "AssetSerializer.h"

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

		std::string vertexData = m_VertexCount > 0 ? VertexDataToHex() : "";
		std::string indexData = m_IndexCount > 0 ? IndexDataToHex() : "";

		root.WriteData("m_VertexCount", m_VertexCount);
		root.WriteData("m_VertexData", vertexData);
		root.WriteData("m_IndexCount", m_IndexCount);
		root.WriteData("m_IndexData", indexData);

		serializer.WriteToDisk(path);
	}

	void Mesh::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			std::string vertexData;
			std::string indexData;
			root.ReadData("m_VertexCount", m_VertexCount);
			root.ReadData("m_VertexData", vertexData);
			root.ReadData("m_IndexCount", m_IndexCount);
			root.ReadData("m_IndexData", indexData);

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