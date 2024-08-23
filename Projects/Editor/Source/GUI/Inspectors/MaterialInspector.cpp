#include "MaterialInspector.h"
#include "Material.h"
#include "Shader.h"
#include "Texture2D.h"
#include "AssetManager.h"
#include "imgui.h"

namespace Odyssey
{
	MaterialInspector::MaterialInspector(GUID guid)
	{
		if (m_Material = AssetManager::LoadMaterialByGUID(guid))
		{
			GUID fragmentShaderGUID;
			GUID vertexShaderGUID;
			GUID textureGUID;

			if (auto fragmentShader = m_Material->GetFragmentShader())
				fragmentShaderGUID = fragmentShader->GetGUID();

			if (auto vertexShader = m_Material->GetVertexShader())
				vertexShaderGUID = vertexShader->GetGUID();

			if (auto texture2D = m_Material->GetTexture())
				textureGUID = texture2D->GetGUID();

			m_NameDrawer = StringDrawer("Name", m_Material->GetName(),
				[this](const std::string& name) { OnNameModified(name); });

			m_GUIDDrawer = StringDrawer("GUID", m_Material->GetGUID().String(), nullptr, true);

			m_FragmentShaderDrawer = AssetFieldDrawer("Fragment Shader", fragmentShaderGUID, "Shader",
				[this](GUID guid) { OnFragmentShaderModified(guid); });

			m_VertexShaderDrawer = AssetFieldDrawer("Vertex Shader", vertexShaderGUID, "Shader",
				[this](GUID guid) { OnVertexShaderModified(guid); });

			m_TextureDrawer = AssetFieldDrawer("Texture", textureGUID, "Texture2D",
				[this](GUID guid) { OnTextureModified(guid); });
		}
	}

	void MaterialInspector::Draw()
	{
		m_GUIDDrawer.Draw();
		m_NameDrawer.Draw();
		m_VertexShaderDrawer.Draw();
		m_FragmentShaderDrawer.Draw();
		m_TextureDrawer.Draw();

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

	void MaterialInspector::OnNameModified(const std::string& name)
	{
		if (m_Material)
			m_Material->SetName(name);
	}

	void MaterialInspector::OnFragmentShaderModified(GUID guid)
	{
		if (m_Material)
		{
			if (auto fragShader = AssetManager::LoadShaderByGUID(guid))
				m_Material->SetFragmentShader(fragShader);
		}
	}

	void MaterialInspector::OnVertexShaderModified(GUID guid)
	{
		if (m_Material)
		{
			if (auto vertShader = AssetManager::LoadShaderByGUID(guid))
				m_Material->SetVertexShader(vertShader);
		}
	}

	void MaterialInspector::OnTextureModified(GUID guid)
	{
		if (m_Material)
		{
			if (auto texture = AssetManager::LoadTexture2DByGUID(guid))
				m_Material->SetTexture(texture);
		}
	}
}