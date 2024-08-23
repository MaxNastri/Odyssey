#pragma once
#include "Inspector.h"
#include "AssetFieldDrawer.h"
#include "StringDrawer.h"

namespace Odyssey
{
	class Texture2D;

	class TextureInspector : public Inspector
	{
	public:
		TextureInspector() = default;
		TextureInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		void OnNameChanged(const std::string& name);
		void OnSourceAssetchanged(GUID sourceGUID);

	private:
		std::shared_ptr<Texture2D> m_Texture;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		StringDrawer m_TypeDrawer;
		StringDrawer m_PixelDataGUID;
		AssetFieldDrawer m_SourceAssetDrawer;

	};
}