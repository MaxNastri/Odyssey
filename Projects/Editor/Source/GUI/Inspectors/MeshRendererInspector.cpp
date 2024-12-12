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

			m_MeshRendererEnabled = meshRenderer->IsEnabled();

			m_MeshDrawer = AssetFieldDrawer("Mesh", meshGUID, Mesh::Type);
			auto& materials = meshRenderer->GetMaterials();
			for (size_t i = 0; i < materials.size(); i++)
			{
				m_MaterialDrawers.emplace_back(AssetFieldDrawer(std::format("Material {}", i), materials[i]->GetGUID(), Material::Type));
			}
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

			for (size_t i = 0; i < m_MaterialDrawers.size(); i++)
			{
				if (m_MaterialDrawers[i].Draw())
				{
					if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
						meshRenderer->SetMaterial(m_MaterialDrawers[i].GetGUID(), i);

					modified = true;
				}
			}

			if (ImGui::Button("Add Material"))
			{
				size_t materialIndex = m_MaterialDrawers.size();
				m_MaterialDrawers.emplace_back(AssetFieldDrawer(std::format("Material {}", materialIndex), 0, Material::Type));
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove Last Material"))
			{
				if (m_MaterialDrawers.size() > 0)
					m_MaterialDrawers.erase(m_MaterialDrawers.end() - 1);

				if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
					meshRenderer->RemoveMaterial();
			}
		}

		ImGui::PopID();

		return modified;
	}
}