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
		Texture2D(const Path& assetPath, Ref<SourceTexture> source);

	public:
		virtual void Save() override;
		void Load(Ref<SourceTexture> source);

	public:
		ResourceID GetTexture() { return m_Texture; }
		uint32_t GetWidth() { return m_TextureDescription.Width; }
		uint32_t GetHeight() { return m_TextureDescription.Height; }
		bool GetMipMapsEnabled() { return m_TextureDescription.MipMapEnabled; }
		float GetMipBias() { return m_TextureDescription.MipBias; }
		uint32_t GetMaxMipCount() { return m_TextureDescription.MaxMipCount; }

	public:
		void SetMipMapsEnabled(bool enabled);
		void SetMipBias(float bias);
		void SetMaxMipCount(uint32_t count);

	private:
		void LoadFromSource(Ref<SourceTexture> source);
		void SaveToDisk(const Path& assetPath);

	private:
		void OnSourceModified();

	private:
		GUID m_PixelBufferGUID;
		VulkanImageDescription m_TextureDescription;
		ResourceID m_Texture;
	};
}