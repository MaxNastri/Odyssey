#include "Texture2D.h"
#include "ResourceManager.h"
#include "VulkanTextureSampler.h"

namespace Odyssey
{
	Texture2D::Texture2D(const std::filesystem::path& assetPath)
		: Asset(assetPath)
	{
		Load();
	}

	Texture2D::Texture2D(const std::filesystem::path& assetPath, TextureFormat format)
		: Asset(assetPath)
	{
		Load();
	}

	void Texture2D::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Texture2D::Load()
	{
		LoadFromDisk(m_AssetPath);
	}

	void Texture2D::SaveToDisk(const std::filesystem::path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		if (m_PixelBuffer.size() > 0)
		{
			std::string pixelDataInHex = PixelDataToHex();
			root.WriteData("m_Width", m_TextureDescription.Width);
			root.WriteData("m_Height", m_TextureDescription.Height);
			root.WriteData("m_PixelData", pixelDataInHex);
		}

		serializer.WriteToDisk(assetPath);
	}

	void Texture2D::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			std::string pixelDataInHex;
			root.ReadData("m_Width", m_TextureDescription.Width);
			root.ReadData("m_Height", m_TextureDescription.Height);
			root.ReadData("m_PixelData", pixelDataInHex);

			m_TextureDescription.Channels = 4;
			m_TextureDescription.Format = TextureFormat::R8G8B8A8_UNORM;

			size_t dataSize = m_TextureDescription.Width * m_TextureDescription.Height * m_TextureDescription.Channels;
			HexToPixelData(pixelDataInHex, dataSize);

			m_Texture = ResourceManager::AllocateTexture(m_TextureDescription, m_PixelBuffer.data());
		}
	}

	std::string Texture2D::PixelDataToHex()
	{
		size_t bufferSize = m_PixelBuffer.size();
		std::vector<unsigned char> buffer(bufferSize);
		memcpy(&buffer[0], m_PixelBuffer.data(), bufferSize);

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

	void Texture2D::HexToPixelData(const std::string& hexData, uint32_t dataSize)
	{
		// Read the hex data into a stream
		std::istringstream stream(hexData);
		stream >> std::hex;

		// Create a buffer for the binary data
		size_t bufferSize = dataSize;
		std::vector<unsigned char> buffer(bufferSize);

		size_t index = 0;
		size_t stride = sizeof(uint16_t);
		for (uint32_t i = 0; i < bufferSize; i++)
		{
			std::string sub = hexData.substr(index, stride);
			buffer[i] = (unsigned char)strtol(sub.c_str(), NULL, 16);
			index += stride;
		}

		m_PixelBuffer.clear();
		m_PixelBuffer.resize(bufferSize);

		// Copy the binary vertex data into the vertices vector
		memcpy(m_PixelBuffer.data(), &buffer[0], bufferSize);

	}
}