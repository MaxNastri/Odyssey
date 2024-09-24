#pragma once
#include "Asset.h"
#include "VulkanGlobals.h"
#include "VulkanImage.h"

namespace Odyssey
{
	class Cubemap : public Asset
	{
	public:
		Cubemap(const Path& assetPath);

	private:
		GUID m_PixelBufferGUID;
		VulkanImageDescription m_TextureDescription;
		ResourceID m_Texture;
	};
}