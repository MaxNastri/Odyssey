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
			GUID shaderGUID;
			GUID textureGUID;

			if (auto shader = m_Material->GetShader())
				shaderGUID = shader->GetGUID();

			if (auto texture2D = m_Material->GetTexture())
				textureGUID = texture2D->GetGUID();

			m_NameDrawer = StringDrawer("Name", m_Material->GetName(),
				[this](const std::string& name) { OnNameModified(name); });

			m_GUIDDrawer = StringDrawer("GUID", m_Material->GetGUID().String(), nullptr, true);

			m_ShaderDrawer = AssetFieldDrawer("Shader", shaderGUID, "Shader",
				[this](GUID guid) { OnShaderModified(guid); });

			m_TextureDrawer = AssetFieldDrawer("Texture", textureGUID, "Texture2D",
				[this](GUID guid) { OnTextureModified(guid); });
		}
	}

	void MaterialInspector::Draw()
	{
		m_GUIDDrawer.Draw();
		m_NameDrawer.Draw();
		m_ShaderDrawer.Draw();
		m_TextureDrawer.Draw();

		if (m_Modified)
		{
			if (ImGui::Button("Save"))
			{
				m_Material->Save();
				m_Modified = false;
			}
		}
	}

	void MaterialInspector::OnNameModified(const std::string& name)
	{
		if (m_Material)
			m_Material->SetName(name);

		m_Modified = true;
	}

	void MaterialInspector::OnShaderModified(GUID guid)
	{
		if (m_Material)
		{
			if (auto vertShader = AssetManager::LoadShaderByGUID(guid))
				m_Material->SetShader(vertShader);

			m_Modified = true;
		}
	}

	void MaterialInspector::OnTextureModified(GUID guid)
	{
		if (m_Material)
		{
			if (auto texture = AssetManager::LoadTexture2DByGUID(guid))
				m_Material->SetTexture(texture);

			m_Modified = true;
		}
	}
}