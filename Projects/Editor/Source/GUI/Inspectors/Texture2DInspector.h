#pragma once
#include "Inspector.h"
#include "Texture2D.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class TextureInspector : public Inspector
	{
	public:
		TextureInspector() = default;
		TextureInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		void OnNameChanged(std::string_view name);
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