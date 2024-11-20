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
		if (m_Material = AssetManager::LoadAsset<Material>(guid))
		{
			GUID shaderGUID;
			GUID textureGUID;

			if (auto shader = m_Material->GetShader())
				shaderGUID = shader->GetGUID();

			if (auto texture2D = m_Material->GetTexture())
				textureGUID = texture2D->GetGUID();

			m_NameDrawer = StringDrawer("Name", m_Material->GetName(),
				[this](std::string_view name) { OnNameModified(name); });

			m_GUIDDrawer = StringDrawer("GUID", m_Material->GetGUID().String(), nullptr, true);

			m_ShaderDrawer = AssetFieldDrawer("Shader", shaderGUID, Shader::Type,
				[this](GUID guid) { OnShaderModified(guid); });

			m_TextureDrawer = AssetFieldDrawer("Texture", textureGUID, Texture2D::Type,
				[this](GUID guid) { OnTextureModified(guid); });
		}
	}

	bool MaterialInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();
		modified |= m_NameDrawer.Draw();
		modified |= m_ShaderDrawer.Draw();
		modified |= m_TextureDrawer.Draw();

		if (modified && ImGui::Button("Save"))
			m_Material->Save();

		return modified;
	}

	void MaterialInspector::OnNameModified(std::string_view name)
	{
		if (m_Material)
			m_Material->SetName(name);
	}

	void MaterialInspector::OnShaderModified(GUID guid)
	{
		if (m_Material)
		{
			if (auto vertShader = AssetManager::LoadAsset<Shader>(guid))
				m_Material->SetShader(vertShader);
		}
	}

	void MaterialInspector::OnTextureModified(GUID guid)
	{
		if (m_Material)
		{
			if (auto texture = AssetManager::LoadAsset<Texture2D>(guid))
				m_Material->SetTexture(texture);
		}
	}
}