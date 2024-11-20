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
				[this](std::string_view assetPath) { OnAssetPathChanged(assetPath); });
		}
	}

	bool SourceTextureInspector::Draw()
	{
		bool modified = false;

		modified |= m_TextureNameDrawer.Draw();
		modified |= m_AssetPathDrawer.Draw();

		if (ImGui::Button("Create Texture2D"))
		{
			AssetManager::CreateAsset<Texture2D>(Project::GetActiveAssetsDirectory() / m_AssetPath, m_Texture);
		}
		if (ImGui::Button("Create Cubemap"))
		{
			AssetManager::CreateAsset<Cubemap>(Project::GetActiveAssetsDirectory() / m_AssetPath, m_Texture);
		}

		return modified;
	}
}