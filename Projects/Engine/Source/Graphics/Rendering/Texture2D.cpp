#include "Texture2D.h"
#include "ResourceManager.h"
#include "VulkanTexture.h"
#include "VulkanTextureSampler.h"
#include "BinaryBuffer.h"
#include "AssetManager.h"
#include "SourceTexture.h"

namespace Odyssey
{
	Texture2D::Texture2D(const Path& assetPath)
		: Asset(assetPath)
	{
		if (auto source = AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset))
		{
			source->AddOnModifiedListener([this]() { OnSourceModified(); });
			LoadFromSource(source);
		}
	}

	Texture2D::Texture2D(const Path& assetPath, TextureFormat format)
		: Asset(assetPath)
	{
		if (auto source = AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset))
		{
			source->AddOnModifiedListener([this]() { OnSourceModified(); });
			LoadFromSource(source);
		}
	}

	Texture2D::Texture2D(const Path& assetPath, std::shared_ptr<SourceTexture> source)
		: Asset(assetPath)
	{
		SetSourceAsset(source->GetGUID());
	}

	void Texture2D::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Texture2D::Load()
	{
		if (auto source = AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset))
			LoadFromSource(source);
		//LoadFromDisk(m_AssetPath);
	}

	void Texture2D::LoadFromSource(std::shared_ptr<SourceTexture> source)
	{
		// Copy in the texture settings
		m_TextureDescription.Width = (uint32_t)source->GetWidth();
		m_TextureDescription.Height = (uint32_t)source->GetHeight();
		m_TextureDescription.Channels = (uint32_t)source->GetChannels();

		// Destroy the existing texture
		if (m_Texture)
			ResourceManager::Destroy(m_Texture);

		// Allocate a new texture using the source pixel buffer
		m_Texture = ResourceManager::Allocate<VulkanTexture>(m_TextureDescription, source->GetPixelBuffer());
	}

	void Texture2D::SaveToDisk(const Path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		root.WriteData("m_Width", m_TextureDescription.Width);
		root.WriteData("m_Height", m_TextureDescription.Height);
		root.WriteData("m_PixelBufferGUID", m_PixelBufferGUID.CRef());

		serializer.WriteToDisk(assetPath);
	}

	void Texture2D::LoadFromDisk(const Path& assetPath)
	{
		AssetDeserializer deserializer(assetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			root.ReadData("m_Width", m_TextureDescription.Width);
			root.ReadData("m_Height", m_TextureDescription.Height);
			root.ReadData("m_PixelBufferGUID", m_PixelBufferGUID.Ref());

			m_TextureDescription.Channels = 4;
			m_TextureDescription.Format = TextureFormat::R8G8B8A8_UNORM;

			BinaryBuffer pixelBuffer = AssetManager::LoadBinaryAsset(m_PixelBufferGUID);
			m_Texture = ResourceManager::Allocate<VulkanTexture>(m_TextureDescription, pixelBuffer);
		}
	}

	void Texture2D::OnSourceModified()
	{
		if (auto source = AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset))
			LoadFromSource(source);
	}
}