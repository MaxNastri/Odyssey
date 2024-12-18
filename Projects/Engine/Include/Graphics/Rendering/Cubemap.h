#pragma once
#include "Asset.h"
#include "VulkanGlobals.h"
#include "VulkanImage.h"
#include "TextureImportSettings.h"

namespace Odyssey
{
	class SourceTexture;

	class Cubemap : public Asset
	{
		CLASS_DECLARATION(Odyssey, Cubemap)
	public:
		Cubemap(const Path& assetPath);
		Cubemap(const Path& assetPath, Ref<SourceTexture> source);

	public:
		virtual void Save() override;
		void Load();

	public:
		ResourceID GetTexture() { return m_Texture; }

	private:
		void LoadFromSource(Ref<SourceTexture> source);
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