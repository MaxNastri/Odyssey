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
			m_GUIDDrawer = StringDrawer("GUID", m_Mesh->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Mesh->GetName(), false,
				[this](std::string_view name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", m_Mesh->GetType(), true);
			m_VertexCountDrawer = StringDrawer("Vertex Count", std::to_string(m_Mesh->GetVertexCount()), true);
			m_IndexCountDrawer = StringDrawer("Index Count", std::to_string(m_Mesh->GetIndexCount()), true);
			m_SourceMeshDrawer = AssetFieldDrawer("Source Asset", m_Mesh->GetSourceAsset(), SourceModel::Type,
				[this](GUID sourceGUID) { OnSourceAssetChanged(sourceGUID); });
		}
	}

	bool MeshInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();
		modified |= m_NameDrawer.Draw();
		modified |= m_TypeDrawer.Draw();
		modified |= m_VertexCountDrawer.Draw();
		modified |= m_IndexCountDrawer.Draw();
		modified |= m_SourceMeshDrawer.Draw();

		return modified;
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