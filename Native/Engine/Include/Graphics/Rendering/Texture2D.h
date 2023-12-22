#pragma once
#include "Asset.h"
#include "Enums.h"
#include "ResourceHandle.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanImage;
	class VulkanTextureSampler;

	class Texture2D : public Asset
	{
	public:
		Texture2D(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath);
		Texture2D(const std::filesystem::path& assetPath, const std::filesystem::path& metaPath, TextureFormat format);
		Texture2D(uint32_t width, uint32_t height, TextureFormat format);

	public:
		void Save();
		void Load();

	public:
		ResourceHandle<VulkanImage> GetImage() { return m_Image; }
		ResourceHandle<VulkanTextureSampler> GetSampler() { return m_Sampler; }

	public:
		void SetSampler(ResourceHandle<VulkanTextureSampler> sampler) { m_Sampler = sampler; }

	private:
		void SaveToDisk(const std::filesystem::path& assetPath);
		void LoadFromDisk(const std::filesystem::path& assetPath);

	private:
		ResourceHandle<VulkanImage> m_Image;
		ResourceHandle<VulkanTextureSampler> m_Sampler;
		uint32_t m_Width, m_Height;
		TextureFormat m_Format;

	};
}