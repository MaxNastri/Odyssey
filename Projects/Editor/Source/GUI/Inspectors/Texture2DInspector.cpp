#include "Texture2DInspector.h"
#include "AssetManager.h"
#include "Texture2D.h"

namespace Odyssey
{
	TextureInspector::TextureInspector(GUID guid)
	{
		m_Texture = AssetManager::LoadTexture2DByGUID(guid);

		if (auto texture = m_Texture.Get())
		{
			m_GUIDDrawer = StringDrawer("GUID", texture->GetGUID().String(), nullptr, true);
			m_NameDrawer = StringDrawer("Name", texture->GetName(),
				[this](const std::string& name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", texture->GetType(), nullptr, true);
			m_PixelDataGUID = StringDrawer("Pixel Data", texture->GetPixelBufferGUID(), nullptr, true);
			m_SourceAssetDrawer = AssetFieldDrawer("Source Asset", texture->GetSoureAsset(), "SourceTexture",
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
		if (auto texture = m_Texture.Get())
		{
			texture->SetName(name);
			texture->Save();
		}
	}
	void TextureInspector::OnSourceAssetchanged(GUID sourceGUID)
	{
		if (auto texture = m_Texture.Get())
		{
			texture->SetSourceAsset(sourceGUID);
			texture->Save();
		}
	}
}