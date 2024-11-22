#include "SourceTexture.h"
#include "AssetManager.h"
#include "RawBuffer.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

namespace Odyssey
{
	SourceTexture::SourceTexture(const Path& sourcePath)
		: SourceAsset(sourcePath)
	{
		TrackingOptions options;
		options.TrackingPath = sourcePath;
		options.Callback = [this](const Path& path, FileActionType fileAction) { OnFileModified(path, fileAction); };
		m_FileTracker = std::make_unique<FileTracker>(options);

		LoadTexture();
	}

	void SourceTexture::LoadTexture()
	{
		uint8_t* pixels = stbi_load(m_SourcePath.string().c_str(), &m_Width, &m_Height, &m_Channels, 4);
		m_Channels = 4;

		size_t bufferSize = sizeof(uint8_t) * m_Width * m_Height * 4;
		m_PixelBuffer.WriteData(pixels, bufferSize);
	}

	void SourceTexture::OnFileModified(const Path& path, FileActionType fileAction)
	{
		if (fileAction == FileActionType::Modified)
		{
			LoadTexture();
			OnSourceModified();
		}
	}
}