#include "SourceModelInspector.h"
#include "AssetManager.h"
#include "SourceModel.h"
#include "imgui.h"
#include "Project.h"
#include "Mesh.h"
#include "AnimationRig.h"
#include "AnimationClip.h"

namespace Odyssey
{
	SourceModelInspector::SourceModelInspector(GUID guid)
	{
		if (m_Model = AssetManager::LoadSourceAsset<SourceModel>(guid))
		{
			m_Drawers.push_back(StringDrawer("Dst Asset Path", "", 
				[this](std::string_view path) { OnDstPathChanged(path); }, false));
		}
	}

	bool SourceModelInspector::Draw()
	{
		bool modified = false;

		for (auto& drawer : m_Drawers)
			modified |= drawer.Draw();

		if (ImGui::Button("Create Mesh Asset"))
		{
			AssetManager::CreateAsset<Mesh>(Project::GetActiveAssetsDirectory() / m_DstPath, m_Model);
		}
		else if (ImGui::Button("Create Rig Asset"))
		{
			AssetManager::CreateAsset<AnimationRig>(Project::GetActiveAssetsDirectory() / m_DstPath, m_Model);
		}
		else if (ImGui::Button("Create Animation Clip"))
		{
			AssetManager::CreateAsset<AnimationClip>(Project::GetActiveAssetsDirectory() / m_DstPath, m_Model);
		}

		return modified;
	}
}