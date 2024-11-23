#pragma once
#include "Inspector.h"
#include "SourceTexture.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class SourceTextureInspector : public Inspector
	{
	public:
		SourceTextureInspector() = default;
		SourceTextureInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnAssetPathChanged(std::string_view assetPath) { m_AssetPath = assetPath; }

	private:
		Ref<SourceTexture> m_Texture;
		StringDrawer m_TextureNameDrawer;
		StringDrawer m_AssetPathDrawer;
		std::string m_AssetPath;
	};
}