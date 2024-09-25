#pragma once
#include "Enums.h"

namespace Odyssey
{
	class TextureImportSettings
	{
	public:
		void SetTextureType(TextureType textureType) { m_TextureType = textureType; }
		void SetCubemapFace(CubemapFace face, GUID texture) { m_CubemapFaces[face] = texture; }

	public:
		GUID GetCubemapFace(CubemapFace face) { return m_CubemapFaces[face]; }

	private:
		TextureType m_TextureType;
		std::unordered_map<CubemapFace, GUID> m_CubemapFaces;
	};
}