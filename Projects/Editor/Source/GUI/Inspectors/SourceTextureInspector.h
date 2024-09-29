#pragma once
#include "Inspector.h"
#include "SourceTexture.h"
#include "AssetFieldDrawer.h"
#include "StringDrawer.h"

namespace Odyssey
{
	class SourceTextureInspector : public Inspector
	{
	public:
		SourceTextureInspector() = default;
		SourceTextureInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		void OnAssetPathChanged(const std::string& assetPath) { m_AssetPath = assetPath; }

	private:
		std::shared_ptr<SourceTexture> m_Texture;
		StringDrawer m_TextureNameDrawer;
		StringDrawer m_AssetPathDrawer;
		std::string m_AssetPath;
	};
}