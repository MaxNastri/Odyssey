#include "SourceTexture.h"
#include "AssetManager.h"
#include "RawBuffer.h"
#include "stb_image.h"

namespace Odyssey
{
	SourceTexture::SourceTexture(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		LoadTexture();
	}


	void SourceTexture::LoadTexture()
	{
		if (std::filesystem::exists(m_SourcePath))
		{
			uint8_t* pixels = stbi_load(m_SourcePath.string().c_str(), &m_Width, &m_Height, &m_Channels, 4);
			m_Channels = 4;

			size_t bufferSize = sizeof(uint8_t) * m_Width * m_Height * 4;
			m_PixelBuffer.WriteData(pixels, bufferSize);
		}
	}
}