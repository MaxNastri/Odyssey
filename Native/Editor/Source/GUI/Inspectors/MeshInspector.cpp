#include "MeshInspector.h"
#include "AssetManager.h"
#include "Mesh.h"

namespace Odyssey
{
	MeshInspector::MeshInspector(const std::string& guid)
	{
		m_Mesh = AssetManager::LoadMeshByGUID(guid);

		if (auto mesh = m_Mesh.Get())
		{
			m_GUIDDrawer = StringDrawer("GUID", mesh->GetGUID(), nullptr, true);
			m_NameDrawer = StringDrawer("Name", mesh->GetName(),
				[this](const std::string& name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", mesh->GetType(), nullptr, true);
			m_VertexCountDrawer = StringDrawer("Vertex Count", std::to_string(mesh->GetVertexCount()), nullptr, true);
			m_IndexCountDrawer = StringDrawer("Index Count", std::to_string(mesh->GetIndexCount()), nullptr, true);
			m_SourceMeshDrawer = AssetFieldDrawer("Source Asset", mesh->GetSoureAsset(), "SourceMesh",
				[this](const std::string& guid) { OnSourceAssetChanged(guid); });
		}
	}
	void MeshInspector::Draw()
	{
		m_GUIDDrawer.Draw();
		m_NameDrawer.Draw();
		m_TypeDrawer.Draw();
		m_VertexCountDrawer.Draw();
		m_IndexCountDrawer.Draw();
		m_SourceMeshDrawer.Draw();
	}
	void MeshInspector::OnNameChanged(const std::string& name)
	{
		if (auto mesh = m_Mesh.Get())
		{
			mesh->SetName(name);
			mesh->Save();
		}
	}
	void MeshInspector::OnSourceAssetChanged(const std::string& guid)
	{
		if (auto mesh = m_Mesh.Get())
		{
			mesh->SetSourceAsset(guid);
			mesh->Save();
		}
	}
}