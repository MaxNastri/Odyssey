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

			m_GUIDDrawer = StringDrawer("GUID", m_Material->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Material->GetName(), false);
			m_ShaderDrawer = AssetFieldDrawer("Shader", shaderGUID, Shader::Type);
			m_TextureDrawer = AssetFieldDrawer("Texture", textureGUID, Texture2D::Type);
		}
	}

	bool MaterialInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();

		if (modified |= m_NameDrawer.Draw())
		{
			m_Material->SetName(m_NameDrawer.GetValue());
			m_Dirty = true;
		}

		if (modified |= m_ShaderDrawer.Draw())
		{
			if (auto shader = AssetManager::LoadAsset<Shader>(m_ShaderDrawer.GetGUID()))
				m_Material->SetShader(shader);
			m_Dirty = true;
		}

		if (modified |= m_TextureDrawer.Draw())
		{
			if (auto texture = AssetManager::LoadAsset<Texture2D>(m_TextureDrawer.GetGUID()))
				m_Material->SetTexture(texture);
			m_Dirty = true;
		}

		if (m_Dirty && ImGui::Button("Save"))
		{
			m_Material->Save();
			m_Dirty = false;
		}

		return modified;
	}
}