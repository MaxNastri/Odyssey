#include "MaterialInspector.h"
#include "Material.h"
#include "Shader.h"
#include "Texture2D.h"
#include "AssetManager.h"
#include "imgui.h"

namespace Odyssey
{
	MaterialInspector::MaterialInspector(Material* material)
	{
		m_Material = material;

		std::string fragmentShaderGUID;
		std::string vertexShaderGUID;
		std::string textureGUID;

		if (Shader* fragmentShader = m_Material->GetFragmentShader().Get())
			fragmentShaderGUID = fragmentShader->GetGUID();

		if (Shader* vertexShader = m_Material->GetVertexShader().Get())
			vertexShaderGUID = vertexShader->GetGUID();

		if (Texture2D* texture2D = m_Material->GetTexture().Get())
			textureGUID = texture2D->GetGUID();

		m_NameDrawer = StringDrawer("Name", m_Material->GetName(),
			[material](const std::string& name) { OnNameModified(material, name); });

		m_GUIDDrawer = ReadOnlyStringDrawer("GUID", m_Material->GetGUID());

		m_FragmentShaderDrawer = AssetFieldDrawer("Fragment Shader", fragmentShaderGUID, "Shader",
			[material](const std::string& guid) { OnFragmentShaderModified(material, guid); });

		m_VertexShaderDrawer = AssetFieldDrawer("Vertex Shader", vertexShaderGUID, "Shader",
			[material](const std::string& guid) { OnVertexShaderModified(material, guid); });

		m_TextureDrawer = AssetFieldDrawer("Texture", textureGUID, "Texture2D",
			[material](const std::string& guid) { OnTextureModified(material, guid); });
	}

	MaterialInspector::MaterialInspector(const std::string& guid)
	{
		AssetHandle<Material> materialHandle = AssetManager::LoadMaterialByGUID(guid);

		if (Material* material = materialHandle.Get())
		{
			m_Material = material;

			std::string fragmentShaderGUID;
			std::string vertexShaderGUID;
			std::string textureGUID;

			if (Shader* fragmentShader = m_Material->GetFragmentShader().Get())
				fragmentShaderGUID = fragmentShader->GetGUID();

			if (Shader* vertexShader = m_Material->GetVertexShader().Get())
				vertexShaderGUID = vertexShader->GetGUID();

			if (Texture2D* texture2D = m_Material->GetTexture().Get())
				textureGUID = texture2D->GetGUID();

			m_NameDrawer = StringDrawer("Name", m_Material->GetName(),
				[material](const std::string& name) { OnNameModified(material, name); });

			m_GUIDDrawer = ReadOnlyStringDrawer("GUID", m_Material->GetGUID());

			m_FragmentShaderDrawer = AssetFieldDrawer("Fragment Shader", fragmentShaderGUID, "Shader",
				[material](const std::string& guid) { OnFragmentShaderModified(material, guid); });

			m_VertexShaderDrawer = AssetFieldDrawer("Vertex Shader", vertexShaderGUID, "Shader",
				[material](const std::string& guid) { OnVertexShaderModified(material, guid); });

			m_TextureDrawer = AssetFieldDrawer("Texture", textureGUID, "Texture2D",
				[material](const std::string& guid) { OnTextureModified(material, guid); });
		}
	}

	void MaterialInspector::Draw()
	{
		if (ImGui::BeginTable("MaterialInspector", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##A", 0, 0.4f);
			m_NameDrawer.Draw();
			ImGui::TableNextRow();
			m_GUIDDrawer.Draw();
			ImGui::TableNextRow();
			m_VertexShaderDrawer.Draw();
			ImGui::TableNextRow();
			m_FragmentShaderDrawer.Draw();
			ImGui::TableNextRow();
			m_TextureDrawer.Draw();
			ImGui::TableNextRow();
			ImGui::EndTable();
			
			bool modified = m_NameDrawer.IsModified() || m_VertexShaderDrawer.IsModified() || m_FragmentShaderDrawer.IsModified();

			if (modified)
			{
				if (ImGui::Button("Save"))
				{
					m_Material->Save();
					m_NameDrawer.SetModified(false);
					m_VertexShaderDrawer.SetModified(false);
					m_FragmentShaderDrawer.SetModified(false);
				}
			}
		}
	}

	void MaterialInspector::OnNameModified(Material* material, const std::string& name)
	{
		if (material)
			material->SetName(name);
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
	void MaterialInspector::OnTextureModified(Material* material, const std::string& guid)
	{
		AssetHandle<Texture2D> texture = AssetManager::LoadTexture2DByGUID(guid);

		if (texture.IsValid())
			material->SetTexture(texture);
	}
}