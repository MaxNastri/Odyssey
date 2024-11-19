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
			GUID meshGUID = meshRenderer->GetMesh();
			GUID materialGUID = meshRenderer->GetMaterial();

			m_MeshRendererEnabled = meshRenderer->IsEnabled();

			m_MeshDrawer = AssetFieldDrawer("Mesh", meshGUID, Mesh::Type,
				[this](GUID guid) { OnMeshModified(guid); });

			m_MaterialDrawer = AssetFieldDrawer("Material", materialGUID, Material::Type,
				[this](GUID guid) { OnMaterialModified(guid); });
		}
	}

	void MeshRendererInspector::Draw()
	{
		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_MeshRendererEnabled))
		{
			if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
				meshRenderer->SetEnabled(m_MeshRendererEnabled);
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_MeshDrawer.Draw();
			m_MaterialDrawer.Draw();
		}

		ImGui::PopID();
	}

	void MeshRendererInspector::OnMeshModified(GUID guid)
	{
		if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
			meshRenderer->SetMesh(guid);
	}

	void MeshRendererInspector::OnMaterialModified(GUID guid)
	{
		if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
		{
			meshRenderer->SetMaterial(guid);
		}
	}
}