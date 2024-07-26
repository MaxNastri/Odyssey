#pragma once
#include "Asset.h"
#include "Enums.h"
#include "ResourceHandle.h"
#include "VulkanGlobals.h"
#include "VulkanImage.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanTexture;

	class Texture2D : public Asset
	{
	public:
		Texture2D(const std::filesystem::path& assetPath);
		Texture2D(const std::filesystem::path& assetPath, TextureFormat format);

	public:
		void Save();
		void Load();

	public:
		ResourceHandle<VulkanTexture> GetTexture() { return m_Texture; }

	private:
		void SaveToDisk(const std::filesystem::path& assetPath);
		void LoadFromDisk(const std::filesystem::path& assetPath);
		
	private:
		VulkanImageDescription m_TextureDescription;
		ResourceHandle<VulkanTexture> m_Texture;
		std::string m_PixelDataGUID;
	};
}