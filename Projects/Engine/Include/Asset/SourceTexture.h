#pragma once
#include "Asset.h"
#include "Enums.h"
#include "TextureImporter.h"
#include "BinaryBuffer.h"

namespace Odyssey
{
	class SourceTexture : public SourceAsset
	{
		CLASS_DECLARATION(Odyssey, SourceTexture)
	public:
		SourceTexture() = default;
		SourceTexture(const Path& sourcePath);

	public:
		BinaryBuffer& GetPixelBuffer() { return m_PixelBuffer; }
		int32_t GetWidth() { return m_Width; }
		int32_t GetHeight() { return m_Height; }
		int32_t GetChannels() { return m_Channels; }

	private:
		void LoadTexture();

	private:
		int32_t m_Width, m_Height, m_Channels;
		BinaryBuffer m_PixelBuffer;
	};
}