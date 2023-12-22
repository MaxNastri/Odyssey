#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ResourceManager.h"

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

		m_Width = (uint32_t)texWidth;
		m_Height = (uint32_t)texHeight;
		m_Format = TextureFormat::R8G8B8A8_UNORM;

		//	if (!pixels)
	//	{
	//		Logger::LogError("[VulkanTexture] Unable to load texture file " + filename);
	//		return;
	//	}

		// Create the resources
		VulkanImageDescription imageDesc;
		imageDesc.Width = m_Width;
		imageDesc.Height = m_Height;
		imageDesc.Format = m_Format;
		m_Image = ResourceManager::AllocateImage(imageDesc);
		m_Image.Get()->SetData(pixels);

		m_Sampler = ResourceManager::AllocateSampler();
	}
}