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
			//const MeshImportData& meshData = m_Model->GetFBXImporter().GetMeshData();
			//m_Drawers.push_back(StringDrawer("Vertex Count", std::to_string(meshData.VertexLists[0].size()), nullptr, true));
			//m_Drawers.push_back(StringDrawer("Index Count", std::to_string(meshData.IndexLists[0].size()), nullptr, true));
			m_Drawers.push_back(StringDrawer("Dst Asset Path", "", 
				[this](const std::string& path) { OnDstPathChanged(path); }, false));
		}
	}

	void SourceModelInspector::Draw()
	{
		for (auto& drawer : m_Drawers)
			drawer.Draw();

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
	}
}