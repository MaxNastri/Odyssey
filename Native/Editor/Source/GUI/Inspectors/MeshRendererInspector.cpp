#include "MeshRendererInspector.h"
#include "imgui.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "AssetManager.h"
#include "GameObject.h"

namespace Odyssey
{
	MeshRendererInspector::MeshRendererInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
		{
			m_MeshDrawer = AssetFieldDrawer("Mesh", meshRenderer->GetMesh().Get()->GetGUID(), "Mesh",
				[this](const std::string& guid) { OnMeshModified(guid); });

			m_MaterialDrawer = AssetFieldDrawer("Material", meshRenderer->GetMaterial().Get()->GetGUID(), "Material",
				[this](const std::string& guid) { OnMaterialModified(guid); });
		}
	}

	void MeshRendererInspector::Draw()
	{
		if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_MeshDrawer.Draw();
			m_MaterialDrawer.Draw();
		}
	}
	void MeshRendererInspector::OnMeshModified(const std::string& guid)
	{
		if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
		{
			meshRenderer->SetMesh(AssetManager::LoadMeshByGUID(guid));
		}
	}

	void MeshRendererInspector::OnMaterialModified( const std::string& guid)
	{
		if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
		{
			meshRenderer->SetMaterial(AssetManager::LoadMaterialByGUID(guid));
		}
	}
}