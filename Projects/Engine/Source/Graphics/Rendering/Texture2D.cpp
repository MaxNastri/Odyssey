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
		if (Ref<SourceTexture> source = AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset))
			Load(source);
	}

	Texture2D::Texture2D(const Path& assetPath, TextureFormat format)
		: Asset(assetPath)
	{
		if (Ref<SourceTexture> source = AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset))
			Load(source);
	}

	Texture2D::Texture2D(const Path& assetPath, Ref<SourceTexture> source)
		: Asset(assetPath)
	{
		SetSourceAsset(source->GetGUID());
	}

	void Texture2D::Save()
	{
		SaveToDisk(m_AssetPath);
	}

	void Texture2D::Load(Ref<SourceTexture> source)
	{
		AssetDeserializer deserializer(m_AssetPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();
			root.ReadData("Mip Maps Enabled", m_TextureDescription.MipMapEnabled);
			root.ReadData("Mip Bias", m_TextureDescription.MipBias);
			root.ReadData("Max Mip Count", m_TextureDescription.MaxMipCount);
		}

		source->AddOnModifiedListener([this]() { OnSourceModified(); });
		LoadFromSource(source);
	}

	void Texture2D::SetMipMapsEnabled(bool enabled)
	{
		if (m_TextureDescription.MipMapEnabled != enabled)
		{
			m_TextureDescription.MipMapEnabled = enabled;
			LoadFromSource(AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset));
		}
	}

	void Texture2D::SetMipBias(float bias)
	{
		if (m_TextureDescription.MipBias != bias)
		{
			m_TextureDescription.MipBias = bias;
			LoadFromSource(AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset));
		}
	}

	void Texture2D::SetMaxMipCount(uint32_t count)
	{
		if (m_TextureDescription.MaxMipCount != count)
		{
			m_TextureDescription.MaxMipCount = count;
			LoadFromSource(AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset));
		}
	}

	void Texture2D::LoadFromSource(Ref<SourceTexture> source)
	{
		// Copy in the texture settings
		m_TextureDescription.Width = (uint32_t)source->GetWidth();
		m_TextureDescription.Height = (uint32_t)source->GetHeight();
		m_TextureDescription.Channels = (uint32_t)source->GetChannels();

		if (m_AssetPath.string().find("normal") != std::string::npos)
			m_TextureDescription.Format = TextureFormat::R8G8B8A8_UNORM;

		// Destroy the existing texture
		if (m_Texture)
			ResourceManager::Destroy(m_Texture);

		// Allocate a new texture using the source pixel buffer
		m_Texture = ResourceManager::Allocate<VulkanTexture>(m_TextureDescription, &(source->GetPixelBuffer()));
	}

	void Texture2D::SaveToDisk(const Path& assetPath)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		// Serialize the asset metadata first
		SerializeMetadata(serializer);

		root.WriteData("Width", m_TextureDescription.Width);
		root.WriteData("Height", m_TextureDescription.Height);
		root.WriteData("Mip Maps Enabled", m_TextureDescription.MipMapEnabled);
		root.WriteData("Mip Bias", m_TextureDescription.MipBias);
		root.WriteData("Max Mip Count", m_TextureDescription.MaxMipCount);

		serializer.WriteToDisk(assetPath);
	}

	void Texture2D::OnSourceModified()
	{
		if (Ref<SourceTexture> source = AssetManager::LoadSourceAsset<SourceTexture>(m_SourceAsset))
			LoadFromSource(source);
	}
}