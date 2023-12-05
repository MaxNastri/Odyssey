#include "Mesh.h"
#include "ResourceManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

namespace Odyssey
{
	Mesh::Mesh(const std::string& assetPath)
	{
		Load(assetPath);

		m_VertexBuffer = ResourceManager::AllocateVertexBuffer(m_Vertices);
		m_IndexBuffer = ResourceManager::AllocateIndexBuffer(m_Indices);
	}

	void Mesh::Save(const std::string& assetPath)
	{
		// Create a tree and root node
		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		// Serialize the base asset data
		root["m_UUID"] << m_UUID;
		root["m_Name"] << m_Name;
		root["m_AssetPath"] << m_AssetPath;
		root["m_Type"] << m_Type;

		// Serialize the mesh-specific data
		root["m_VertexCount"] << m_VertexCount;
		root["m_VertexData"] << VertexDataToHex();
		root["m_IndexCount"] << m_IndexCount;
		root["m_IndexData"] << IndexDataToHex();

		// Save to disk
		FILE* file2 = fopen(m_AssetPath.c_str(), "w+");
		size_t len = ryml::emit_yaml(tree, tree.root_id(), file2);
		fclose(file2);
	}

	void Mesh::Load(const std::string& assetPath)
	{
		if (std::ifstream ifs{ assetPath })
		{
			// Create the yaml root node
			std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
			ryml::NodeRef node = tree.rootref();

			// Deserialize the base asset data
			node["m_UUID"] >> m_UUID;
			node["m_Name"] >> m_Name;
			node["m_AssetPath"] >> m_AssetPath;
			node["m_Type"] >> m_Type;

			// Deserialize the mesh-specific data
			std::string vertexData;
			std::string indexData;
			node["m_VertexCount"] >> m_VertexCount;
			node["m_VertexData"] >> vertexData;
			node["m_IndexCount"] >> m_IndexCount;
			node["m_IndexData"] >> indexData;

			// Convert the vertex/index data from hex into real values
			HexToVertexData(vertexData, m_VertexCount);
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
			uint16_t b = static_cast<uint16_t>(buffer[i]);
			stream << std::setw(sizeof(uint16_t) * 2) << b;
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

		// Create a char buffer so we can read in each hex value
		std::vector<char> hexBuffer(sizeof(uint16_t) * 2);

		for (uint16_t i = 0; i < bufferSize; i++)
		{
			// Read in a hex value
			stream.read(hexBuffer.data(), sizeof(uint16_t) * 2);

			uint16_t value;

			// Use another stringstream to parse the individual hex value
			std::stringstream local(hexBuffer.data());
			local >> std::hex >> std::setw(sizeof(uint16_t) * 2) >> value;

			// Write the binary value into the buffer
			buffer[i] = static_cast<unsigned char>(value);
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
			stream << std::setw(sizeof(uint16_t) * 2) << static_cast<uint16_t>(buffer[i]);
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

		// Create a char buffer so we can read in each hex value
		std::vector<char> hexBuffer(sizeof(uint16_t) * 2);

		for (uint16_t i = 0; i < bufferSize; i++)
		{
			// Read in a hex value
			stream.read(hexBuffer.data(), sizeof(uint16_t) * 2);

			uint16_t value;

			// Use another stringstream to parse the individual hex value
			std::stringstream local(hexBuffer.data());
			local >> std::hex >> std::setw(sizeof(uint16_t) * 2) >> value;

			// Write the binary value into the buffer
			buffer[i] = static_cast<unsigned char>(value);
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

		m_VertexBuffer = ResourceManager::AllocateVertexBuffer(m_Vertices);
	}

	void Mesh::SetIndices(std::vector<uint32_t>& indices)
	{
		m_Indices = indices;
		m_IndexCount = (uint32_t)m_Indices.size();

		m_IndexBuffer = ResourceManager::AllocateIndexBuffer(m_Indices);
	}
}