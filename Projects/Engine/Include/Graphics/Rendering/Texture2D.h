#pragma once
#include "Asset.h"
#include "Enums.h"
#include "Resource.h"
#include "VulkanGlobals.h"
#include "VulkanImage.h"

namespace Odyssey
{
	class VulkanBuffer;
	class VulkanTexture;

	class Texture2D : public Asset
	{
	public:
		Texture2D(const Path& assetPath);
		Texture2D(const Path& assetPath, TextureFormat format);

	public:
		void Save();
		void Load();

	public:
		ResourceID GetTexture() { return m_Texture; }
		GUID GetPixelBufferGUID() { return m_PixelBufferGUID; }

	private:
		void SaveToDisk(const Path& assetPath);
		void LoadFromDisk(const Path& assetPath);
		
	private:
		GUID m_PixelBufferGUID;
		VulkanImageDescription m_TextureDescription;
		ResourceID m_Texture;
	};
}