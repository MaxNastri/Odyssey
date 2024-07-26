#pragma once
#include "Inspector.h"
#include "AssetHandle.h"
#include "AssetFieldDrawer.h"
#include "StringDrawer.h"

namespace Odyssey
{
	class Texture2D;

	class TextureInspector : public Inspector
	{
	public:
		TextureInspector() = default;
		TextureInspector(const std::string& guid);

	public:
		virtual void Draw() override;

	private:
		void OnNameChanged(const std::string& name);
		void OnSourceAssetchanged(const std::string& asset);

	private:
		AssetHandle<Texture2D> m_Texture;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		StringDrawer m_TypeDrawer;
		StringDrawer m_PixelDataGUID;
		AssetFieldDrawer m_SourceAssetDrawer;

	};
}