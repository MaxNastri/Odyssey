#pragma once
#include "Asset.h"
#include "Enums.h"
#include "Resource.h"
#include "VulkanGlobals.h"
#include "VulkanImage.h"

namespace Odyssey
{
	class SourceTexture;

	class Texture2D : public Asset
	{
		CLASS_DECLARATION(Odyssey, Texture2D)
	public:
		Texture2D(const Path& assetPath);
		Texture2D(const Path& assetPath, TextureFormat format);
		Texture2D(const Path& assetPath, std::shared_ptr<SourceTexture> source);

	public:
		void Save();
		void Load();

	public:
		ResourceID GetTexture() { return m_Texture; }
		GUID GetPixelBufferGUID() { return m_PixelBufferGUID; }

	private:
		void LoadFromSource(std::shared_ptr<SourceTexture> source);
		void SaveToDisk(const Path& assetPath);
		void LoadFromDisk(const Path& assetPath);

	private:
		void OnSourceModified();

	private:
		GUID m_PixelBufferGUID;
		VulkanImageDescription m_TextureDescription;
		ResourceID m_Texture;
	};
}