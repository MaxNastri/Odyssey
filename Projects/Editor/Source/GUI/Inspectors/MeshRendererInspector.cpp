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
			GUID meshGUID = meshRenderer->GetMesh() ? meshRenderer->GetMesh()->GetGUID() : GUID(0);
			GUID materialGUID = meshRenderer->GetMaterial() ? meshRenderer->GetMaterial()->GetGUID() : GUID(0);

			m_MeshRendererEnabled = meshRenderer->IsEnabled();

			m_MeshDrawer = AssetFieldDrawer("Mesh", meshGUID, Mesh::Type);
			m_MaterialDrawer = AssetFieldDrawer("Material", materialGUID, Material::Type);
		}
	}

	bool MeshRendererInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_MeshRendererEnabled))
		{
			if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
				meshRenderer->SetEnabled(m_MeshRendererEnabled);

			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_MeshDrawer.Draw())
			{
				if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
					meshRenderer->SetMesh(m_MeshDrawer.GetGUID());

				modified = true;
			}
			if (m_MaterialDrawer.Draw())
			{
				if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
					meshRenderer->SetMaterial(m_MaterialDrawer.GetGUID());

				modified = true;

			}
		}

		ImGui::PopID();

		return modified;
	}
}