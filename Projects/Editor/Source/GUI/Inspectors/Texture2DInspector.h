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
		virtual bool Draw() override;

	private:
		void OnNameChanged(std::string_view name);
		void OnSourceAssetchanged(GUID sourceGUID);

	private:
		Ref<Texture2D> m_Texture;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		StringDrawer m_TypeDrawer;
		AssetFieldDrawer m_SourceAssetDrawer;
		BoolDrawer m_MipMapDrawer;
		FloatDrawer m_MipBiasDrawer;
		IntDrawer<uint32_t> m_MaxMipCountDrawer;
		uint64_t m_PreviewTexture;
	};
}