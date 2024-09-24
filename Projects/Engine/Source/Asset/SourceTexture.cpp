#include "SourceTexture.h"
#include "AssetManager.h"
#include "RawBuffer.hpp"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

namespace Odyssey
{
	SourceTexture::SourceTexture(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		uint8_t* pixels = stbi_load(sourcePath.string().c_str(), &m_Width, &m_Height, &m_Channels, 4);
		m_Channels = 4;
		size_t bufferSize = sizeof(uint8_t) * m_Width * m_Height * 4;

		m_PixelBuffer.WriteData(pixels, bufferSize);
	}
}