#include "MeshRendererInspector.h"
#include "imgui.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "AssetManager.h"
#include "GameObject.h"
#include "Preferences.h"

namespace Odyssey
{
	MeshRendererInspector::MeshRendererInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;
		InitDrawers();
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

			struct Data
			{
				GUID Mesh;
				GUID* Materials = nullptr;
				size_t MaterialCount = 0;

				~Data()
				{
					if (Materials)
					{
						delete Materials;
						Materials = nullptr;
					}
					MaterialCount = 0;
				}
			};

			if (ImGui::Button("Copy"))
			{
				Data data;
				data.Mesh = m_MeshDrawer.GetGUID();
				data.MaterialCount = m_MaterialDrawers.size();
				data.Materials = new GUID[data.MaterialCount];

				for (size_t i = 0; i < m_MaterialDrawers.size(); i++)
				{
					data.Materials[i] = m_MaterialDrawers[i].GetGUID();
				}

				ClipBoard::Copy("Mesh Renderer", &data, sizeof(data));
			}

			ImGui::SameLine();

			if (ImGui::Button("Paste"))
			{
				if (ClipBoard::GetContext() == "Mesh Renderer")
				{
					if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
					{
						Data data = ClipBoard::Paste().Read<Data>();
						meshRenderer->SetMesh(data.Mesh);

						for (size_t i = 0; i < data.MaterialCount; i++)
						{
							meshRenderer->SetMaterial(data.Materials[i], i);
						}

						InitDrawers();
					}
				}
			}
		}

		ImGui::PopID();

		return modified;
	}

	void MeshRendererInspector::InitDrawers()
	{
		if (MeshRenderer* meshRenderer = m_GameObject.TryGetComponent<MeshRenderer>())
		{
			GUID meshGUID = meshRenderer->GetMesh() ? meshRenderer->GetMesh()->GetGUID() : GUID(0);

			m_MeshRendererEnabled = meshRenderer->IsEnabled();

			m_MeshDrawer = AssetFieldDrawer("Mesh", meshGUID, Mesh::Type);
			m_MaterialDrawers.clear();

			auto& materials = meshRenderer->GetMaterials();
			for (size_t i = 0; i < materials.size(); i++)
			{
				m_MaterialDrawers.emplace_back(AssetFieldDrawer(std::format("Material {}", i), materials[i]->GetGUID(), Material::Type));
			}
		}
	}
}