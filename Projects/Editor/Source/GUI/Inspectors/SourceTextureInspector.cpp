#include "SourceTextureInspector.h"
#include "SourceTexture.h"
#include "imgui.h"
#include "AssetManager.h"
#include "Project.h"

namespace Odyssey
{
	SourceTextureInspector::SourceTextureInspector(GUID guid)
	{
		if (m_Texture = AssetManager::LoadSourceTexture(guid))
		{
			m_TextureNameDrawer = StringDrawer("Texture Name", m_Texture->GetName(), nullptr, true);

			m_AssetPathDrawer = StringDrawer("Asset Path", m_AssetPath,
				[this](const std::string& assetPath) { OnAssetPathChanged(assetPath); });
		}
	}

	void SourceTextureInspector::Draw()
	{
		m_TextureNameDrawer.Draw();
		m_AssetPathDrawer.Draw();

		if (ImGui::Button("Create Texture2D"))
		{
			AssetManager::CreateTexture2D(Project::GetActiveAssetsDirectory() / m_AssetPath, m_Texture);
		}
	}
}