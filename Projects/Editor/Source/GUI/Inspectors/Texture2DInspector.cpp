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
			m_GUIDDrawer = StringDrawer("GUID", m_Texture->GetGUID().String(), nullptr, true);
			m_NameDrawer = StringDrawer("Name", m_Texture->GetName(),
				[this](std::string_view name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", m_Texture->GetType(), nullptr, true);
			m_SourceAssetDrawer = AssetFieldDrawer("Source Asset", m_Texture->GetSourceAsset(), SourceTexture::Type,
				[this](GUID sourceGUID) { OnSourceAssetchanged(sourceGUID); });
		}
	}
	void TextureInspector::Draw()
	{
		m_GUIDDrawer.Draw();
		m_TypeDrawer.Draw();
		m_NameDrawer.Draw();
		m_SourceAssetDrawer.Draw();
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