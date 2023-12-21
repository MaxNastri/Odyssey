#pragma once
#include "Asset.h"
#include "Enums.h"
#include "ResourceHandle.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanImage;

	class Texture2D : public Asset
	{
	public:
		Texture2D(const std::filesystem::path assetPath);
		Texture2D(const std::filesystem::path assetPath, TextureFormat format);
		Texture2D(uint32_t width, uint32_t height, TextureFormat format);

	public:
		void Save();
		void SaveTo(const std::string& assetPath);
		void Load(const std::string& assetPath);

	private:
		ResourceHandle<VulkanImage> m_Image;
		ResourceHandle<VulkanBuffer> m_StagingBuffer;
		uint32_t m_Width, m_Height;

	};
}