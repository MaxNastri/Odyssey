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
			GUID colorTextureGUID;
			GUID normalTextureGUID;

			if (Ref<Shader> shader = m_Material->GetShader())
				shaderGUID = shader->GetGUID();

			if (Ref<Texture2D> texture2D = m_Material->GetColorTexture())
				colorTextureGUID = texture2D->GetGUID();

			if (Ref<Texture2D> normalTexture = m_Material->GetNormalTexture())
				normalTextureGUID = normalTexture->GetGUID();

			m_GUIDDrawer = StringDrawer("GUID", m_Material->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Material->GetName(), false);
			m_ShaderDrawer = AssetFieldDrawer("Shader", shaderGUID, Shader::Type);
			m_ColorTextureDrawer = AssetFieldDrawer("Color Texture", colorTextureGUID, Texture2D::Type);
			m_NormalTextureDrawer = AssetFieldDrawer("Normal Texture", normalTextureGUID, Texture2D::Type);
		}
	}

	bool MaterialInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();

		if (m_NameDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;
		}

		if (m_ShaderDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;

			if (auto shader = AssetManager::LoadAsset<Shader>(m_ShaderDrawer.GetGUID()))
				m_Material->SetShader(shader);
		}

		if (m_ColorTextureDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;

			if (auto texture = AssetManager::LoadAsset<Texture2D>(m_ColorTextureDrawer.GetGUID()))
				m_Material->SetColorTexture(texture);
		}

		if (m_NormalTextureDrawer.Draw())
		{
			m_Dirty = true;
			modified = true;

			if (auto texture = AssetManager::LoadAsset<Texture2D>(m_NormalTextureDrawer.GetGUID()))
				m_Material->SetNormalTexture(texture);
		}

		if (m_Dirty && ImGui::Button("Save"))
		{
			if (m_Material->GetName() != m_NameDrawer.GetValue())
				m_Material->SetName(m_NameDrawer.GetValue());
			
			m_Material->Save();
			m_Dirty = false;
		}

		return modified;
	}
}