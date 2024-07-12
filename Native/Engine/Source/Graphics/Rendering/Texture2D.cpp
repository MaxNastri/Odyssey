#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ResourceManager.h"
#include "VulkanTextureSampler.h"

namespace Odyssey
{
	Texture2D::Texture2D(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath)
		: Asset(assetPath, metaPath)
	{
		Load();
	}

	Texture2D::Texture2D(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath, TextureFormat format)
		: Asset(assetPath, metaPath)
	{
		Load();
	}

	void Texture2D::Save()
	{
		SaveMetadata();
		SaveToDisk(m_AssetPath);
	}

	void Texture2D::Load()
	{
		LoadMetadata();
		LoadFromDisk(m_AssetPath);
	}

	void Texture2D::SaveToDisk(const std::filesystem::path& assetPath)
	{

	}

	void Texture2D::LoadFromDisk(const std::filesystem::path& assetPath)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(assetPath.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (pixels == nullptr)
		{
			Logger::LogError("[VulkanTexture] Unable to load texture from file.");
			return;
		}

		m_TextureDescription.Width = (uint32_t)texWidth;
		m_TextureDescription.Height = (uint32_t)texHeight;
		m_TextureDescription.Format = TextureFormat::R8G8B8A8_UNORM;
		
		m_Texture = ResourceManager::AllocateTexture(m_TextureDescription, pixels);
	}
}