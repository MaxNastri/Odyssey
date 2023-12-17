#include "MaterialInspector.h"
#include "Material.h"
#include "Shader.h"
#include "AssetManager.h"
#include "imgui.h"

namespace Odyssey
{
	MaterialInspector::MaterialInspector(Material* material)
	{
		m_Material = material;

		std::string fragmentShaderGUID = m_Material->GetFragmentShader().Get()->GetGUID();
		std::string vertexShaderGUID = m_Material->GetVertexShader().Get()->GetGUID();

		m_FragmentShaderDrawer = AssetFieldDrawer("Fragment Shader", fragmentShaderGUID,
			[material](const std::string& guid) { OnFragmentShaderModified(material, guid); });

		m_VertexShaderDrawer = AssetFieldDrawer("Vertex Shader", fragmentShaderGUID,
			[material](const std::string& guid) { OnVertexShaderModified(material, guid); });
	}

	MaterialInspector::MaterialInspector(const std::string& guid)
	{
		AssetHandle<Material> material = AssetManager::LoadMaterialByGUID(guid);

		if (material.IsValid())
		{
			Material* materialPtr = material.Get();
			m_Material = material.Get();

			std::string fragmentShaderGUID = m_Material->GetFragmentShader().Get()->GetGUID();
			std::string vertexShaderGUID = m_Material->GetVertexShader().Get()->GetGUID();

			m_FragmentShaderDrawer = AssetFieldDrawer("Fragment Shader", fragmentShaderGUID,
				[materialPtr](const std::string& guid) { OnFragmentShaderModified(materialPtr, guid); });

			m_VertexShaderDrawer = AssetFieldDrawer("Vertex Shader", fragmentShaderGUID,
				[materialPtr](const std::string& guid) { OnVertexShaderModified(materialPtr, guid); });
		}
	}

	void MaterialInspector::Draw()
	{
		if (ImGui::BeginTable("MeshRendererInspector", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##A", 0, 0.4f);
			m_VertexShaderDrawer.Draw();
			ImGui::TableNextRow();
			m_FragmentShaderDrawer.Draw();
			ImGui::TableNextRow();
			ImGui::EndTable();
		}
	}

	void MaterialInspector::OnFragmentShaderModified(Material* material, const std::string& guid)
	{
		AssetHandle<Shader> fragShader = AssetManager::LoadShaderByGUID(guid);

		if (fragShader.IsValid())
			material->SetFragmentShader(fragShader);
	}

	void MaterialInspector::OnVertexShaderModified(Material* material, const std::string& guid)
	{
		AssetHandle<Shader> vertShader = AssetManager::LoadShaderByGUID(guid);

		if (vertShader.IsValid())
			material->SetVertexShader(vertShader);
	}
}