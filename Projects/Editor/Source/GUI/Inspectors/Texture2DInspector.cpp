#include "Texture2DInspector.h"
#include "AssetManager.h"
#include "Texture2D.h"

namespace Odyssey
{
	TextureInspector::TextureInspector(GUID guid)
	{
		if (m_Texture = AssetManager::LoadTexture2DByGUID(guid))
		{
			m_GUIDDrawer = StringDrawer("GUID", m_Texture->GetGUID().String(), nullptr, true);
			m_NameDrawer = StringDrawer("Name", m_Texture->GetName(),
				[this](const std::string& name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", m_Texture->GetType(), nullptr, true);
			m_PixelDataGUID = StringDrawer("Pixel Data", m_Texture->GetPixelBufferGUID().String(), nullptr, true);
			m_SourceAssetDrawer = AssetFieldDrawer("Source Asset", m_Texture->GetSoureAsset(), "SourceTexture",
				[this](GUID sourceGUID) { OnSourceAssetchanged(sourceGUID); });
		}
	}
	void TextureInspector::Draw()
	{
		m_GUIDDrawer.Draw();
		m_TypeDrawer.Draw();
		m_NameDrawer.Draw();
		m_PixelDataGUID.Draw();
		m_SourceAssetDrawer.Draw();
	}

	void TextureInspector::OnNameChanged(const std::string& name)
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