#include "Texture2D.h"
#include "ResourceManager.h"
#include "VulkanTextureSampler.h"
#include "BinaryBuffer.h"
#include "AssetManager.h"

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

		root.WriteData("m_Width", m_TextureDescription.Width);
		root.WriteData("m_Height", m_TextureDescription.Height);
		root.WriteData("m_PixelDataGUID", m_PixelDataGUID);

		serializer.WriteToDisk(assetPath);
	}

	void Texture2D::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			root.ReadData("m_Width", m_TextureDescription.Width);
			root.ReadData("m_Height", m_TextureDescription.Height);
			root.ReadData("m_PixelDataGUID", m_PixelDataGUID);

			m_TextureDescription.Channels = 4;
			m_TextureDescription.Format = TextureFormat::R8G8B8A8_UNORM;

			BinaryBuffer pixelBuffer = AssetManager::LoadBinaryAsset(m_PixelDataGUID);
			m_Texture = ResourceManager::AllocateTexture(m_TextureDescription, pixelBuffer);
		}
	}
}