#include "SourceModelInspector.h"
#include "AssetManager.h"
#include "SourceModel.h"
#include "imgui.h"
#include "Project.h"

namespace Odyssey
{
	SourceModelInspector::SourceModelInspector(GUID guid)
	{
		if (m_Model = AssetManager::LoadSourceModel(guid))
		{
			const size_t meshCount = m_Model->GetImporter().GetMeshCount();
			const MeshImportData& meshData = m_Model->GetImporter().GetMeshData(0);
			m_Drawers.push_back(StringDrawer("Mesh Count", std::to_string(meshCount), nullptr, true));
			m_Drawers.push_back(StringDrawer("Vertex Count", std::to_string(meshData.VertexCount), nullptr, true));
			m_Drawers.push_back(StringDrawer("Index Count", std::to_string(meshData.IndexCount), nullptr, true));
			m_Drawers.push_back(StringDrawer("Normals Count", std::to_string(meshData.NormalsCount), nullptr, true));
			m_Drawers.push_back(StringDrawer("Tangents Count", std::to_string(meshData.TangentsCount), nullptr, true));
			m_Drawers.push_back(StringDrawer("UV Channels", std::to_string(meshData.UVChannelCount), nullptr, true));
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
			AssetManager::CreateMesh(Project::GetActiveAssetsDirectory() / m_DstPath, m_Model);
		}
	}
}