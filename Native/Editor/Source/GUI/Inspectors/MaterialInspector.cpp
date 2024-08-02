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

		GUID fragmentShaderGUID;
		GUID vertexShaderGUID;
		GUID textureGUID;

		if (Shader* fragmentShader = m_Material->GetFragmentShader().Get())
			fragmentShaderGUID = fragmentShader->GetGUID();

		if (Shader* vertexShader = m_Material->GetVertexShader().Get())
			vertexShaderGUID = vertexShader->GetGUID();

		if (Texture2D* texture2D = m_Material->GetTexture().Get())
			textureGUID = texture2D->GetGUID();

		m_NameDrawer = StringDrawer("Name", m_Material->GetName(),
			[material](const std::string& name) { OnNameModified(material, name); });

		m_GUIDDrawer = StringDrawer("GUID", m_Material->GetGUID().String(), nullptr, true);

		m_FragmentShaderDrawer = AssetFieldDrawer("Fragment Shader", fragmentShaderGUID, "Shader",
			[material](GUID guid) { OnFragmentShaderModified(material, guid); });

		m_VertexShaderDrawer = AssetFieldDrawer("Vertex Shader", vertexShaderGUID, "Shader",
			[material](GUID guid) { OnVertexShaderModified(material, guid); });

		m_TextureDrawer = AssetFieldDrawer("Texture", textureGUID, "Texture2D",
			[material](GUID guid) { OnTextureModified(material, guid); });
	}

	MaterialInspector::MaterialInspector(GUID guid)
	{
		AssetHandle<Material> materialHandle = AssetManager::LoadMaterialByGUID(guid);

		if (Material* material = materialHandle.Get())
		{
			m_Material = material;

			GUID fragmentShaderGUID;
			GUID vertexShaderGUID;
			GUID textureGUID;

			if (Shader* fragmentShader = m_Material->GetFragmentShader().Get())
				fragmentShaderGUID = fragmentShader->GetGUID();

			if (Shader* vertexShader = m_Material->GetVertexShader().Get())
				vertexShaderGUID = vertexShader->GetGUID();

			if (Texture2D* texture2D = m_Material->GetTexture().Get())
				textureGUID = texture2D->GetGUID();

			m_NameDrawer = StringDrawer("Name", m_Material->GetName(),
				[material](const std::string& name) { OnNameModified(material, name); });

			m_GUIDDrawer = StringDrawer("GUID", m_Material->GetGUID().String(), nullptr, true);

			m_FragmentShaderDrawer = AssetFieldDrawer("Fragment Shader", fragmentShaderGUID, "Shader",
				[material](GUID guid) { OnFragmentShaderModified(material, guid); });

			m_VertexShaderDrawer = AssetFieldDrawer("Vertex Shader", vertexShaderGUID, "Shader",
				[material](GUID guid) { OnVertexShaderModified(material, guid); });

			m_TextureDrawer = AssetFieldDrawer("Texture", textureGUID, "Texture2D",
				[material](GUID guid) { OnTextureModified(material, guid); });
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

	void MaterialInspector::OnNameModified(Material* material, const std::string& name)
	{
		if (material)
			material->SetName(name);
	}

	void MaterialInspector::OnFragmentShaderModified(Material* material, GUID guid)
	{
		AssetHandle<Shader> fragShader = AssetManager::LoadShaderByGUID(guid);

		if (fragShader.IsValid())
			material->SetFragmentShader(fragShader);
	}

	void MaterialInspector::OnVertexShaderModified(Material* material, GUID guid)
	{
		AssetHandle<Shader> vertShader = AssetManager::LoadShaderByGUID(guid);

		if (vertShader.IsValid())
			material->SetVertexShader(vertShader);
	}
	void MaterialInspector::OnTextureModified(Material* material, GUID guid)
	{
		AssetHandle<Texture2D> texture = AssetManager::LoadTexture2DByGUID(guid);

		if (texture.IsValid())
			material->SetTexture(texture);
	}
}