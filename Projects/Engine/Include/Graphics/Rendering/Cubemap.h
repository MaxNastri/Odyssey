#pragma once
#include "Asset.h"
#include "VulkanGlobals.h"
#include "VulkanImage.h"
#include "TextureImportSettings.h"

namespace Odyssey
{
	class Cubemap : public Asset
	{
	public:
		Cubemap(const Path& assetPath, TextureImportSettings& settings);

	public:
		void Save();
		void Load();

	private:
		void SaveToDisk(const Path& assetPath);
		void LoadFromDisk(const Path& assetPath);

	private:
		GUID m_PixelBufferGUID;
		VulkanImageDescription m_TextureDescription;
		ResourceID m_Texture;
	};
}