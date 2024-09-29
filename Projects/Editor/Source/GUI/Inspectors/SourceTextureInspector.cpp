#include "SourceTextureInspector.h"
#include "SourceTexture.h"
#include "imgui.h"
#include "AssetManager.h"
#include "Project.h"
#include "Texture2D.h"
#include "Cubemap.h"

namespace Odyssey
{
	SourceTextureInspector::SourceTextureInspector(GUID guid)
	{
		if (m_Texture = AssetManager::LoadSourceAsset<SourceTexture>(guid))
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
			AssetManager::CreateAsset<Texture2D>(Project::GetActiveAssetsDirectory() / m_AssetPath, m_Texture);
		}
		if (ImGui::Button("Create Cubemap"))
		{
			AssetManager::CreateAsset<Cubemap>(Project::GetActiveAssetsDirectory() / m_AssetPath, m_Texture);
		}
	}
}