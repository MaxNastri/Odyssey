#include "Texture2DInspector.h"
#include "AssetManager.h"
#include "Texture2D.h"
#include "SourceTexture.h"

namespace Odyssey
{
	TextureInspector::TextureInspector(GUID guid)
	{
		if (m_Texture = AssetManager::LoadAsset<Texture2D>(guid))
		{
			m_GUIDDrawer = StringDrawer("GUID", m_Texture->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Texture->GetName(), false,
				[this](std::string_view name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", m_Texture->GetType(), true);
			m_SourceAssetDrawer = AssetFieldDrawer("Source Asset", m_Texture->GetSourceAsset(), SourceTexture::Type,
				[this](GUID sourceGUID) { OnSourceAssetchanged(sourceGUID); });
		}
	}

	bool TextureInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();
		modified |= m_TypeDrawer.Draw();
		modified |= m_NameDrawer.Draw();
		modified |= m_SourceAssetDrawer.Draw();

		return modified;
	}

	void TextureInspector::OnNameChanged(std::string_view name)
	{
		if (m_Texture)
		{
			m_Texture->SetName(name);
			m_Texture->Save();
		}
	}
	void TextureInspector::OnSourceAssetchanged(GUID sourceGUID)
	{
		if (m_Texture)
		{
			m_Texture->SetSourceAsset(sourceGUID);
			m_Texture->Save();
		}
	}
}