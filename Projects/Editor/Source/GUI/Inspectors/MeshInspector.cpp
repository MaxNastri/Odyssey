#include "MeshInspector.h"
#include "AssetManager.h"
#include "Mesh.h"
#include "SourceModel.h"

namespace Odyssey
{
	MeshInspector::MeshInspector(GUID guid)
	{
		if (m_Mesh = AssetManager::LoadAsset<Mesh>(guid))
		{
			m_GUIDDrawer = StringDrawer("GUID", m_Mesh->GetGUID().String(), nullptr, true);
			m_NameDrawer = StringDrawer("Name", m_Mesh->GetName(),
				[this](std::string_view name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", m_Mesh->GetType(), nullptr, true);
			m_VertexCountDrawer = StringDrawer("Vertex Count", std::to_string(m_Mesh->GetVertexCount()), nullptr, true);
			m_IndexCountDrawer = StringDrawer("Index Count", std::to_string(m_Mesh->GetIndexCount()), nullptr, true);
			m_SourceMeshDrawer = AssetFieldDrawer("Source Asset", m_Mesh->GetSoureAsset(), SourceModel::Type,
				[this](GUID sourceGUID) { OnSourceAssetChanged(sourceGUID); });
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
	void MeshInspector::OnNameChanged(std::string_view name)
	{
		if (m_Mesh)
		{
			m_Mesh->SetName(name);
			m_Mesh->Save();
		}
	}
	void MeshInspector::OnSourceAssetChanged(GUID sourceGUID)
	{
		if (m_Mesh)
		{
			m_Mesh->SetSourceAsset(sourceGUID);
			m_Mesh->Save();
		}
	}
}