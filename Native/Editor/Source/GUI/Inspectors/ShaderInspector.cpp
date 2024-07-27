#include "ShaderInspector.h"
#include "AssetManager.h"
#include "Shader.h"

namespace Odyssey
{
	ShaderInspector::ShaderInspector(const std::string& guid)
	{
		m_Shader = AssetManager::LoadShaderByGUID(guid);

		if (auto shader = m_Shader.Get())
		{
			m_GUIDDrawer = StringDrawer("GUID", shader->GetGUID(), nullptr, true);
			m_NameDrawer = StringDrawer("Name", shader->GetName(), 
				[this](const std::string& name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", shader->GetType(), nullptr, true);
			m_ShaderCodeDrawer = StringDrawer("Shader Code", shader->GetShaderCodeGUID(), nullptr, true);
			m_ShaderTypeDrawer = IntDrawer<uint32_t>("Shader Type", (uint32_t)shader->GetShaderType(), nullptr, true);
			m_SourceShaderDrawer = AssetFieldDrawer("Source Asset", shader->GetSoureAsset(), "SourceShader",
				[this](const std::string& asset) { OnSourceAssetChanged(asset); });
		}
	}

	void ShaderInspector::Draw()
	{
		m_GUIDDrawer.Draw();
		m_NameDrawer.Draw();
		m_TypeDrawer.Draw();
		m_ShaderCodeDrawer.Draw();
		m_ShaderTypeDrawer.Draw();
		m_SourceShaderDrawer.Draw();
	}
	void ShaderInspector::OnNameChanged(const std::string& name)
	{
		if (auto shader = m_Shader.Get())
		{
			shader->SetName(name);
			shader->Save();
		}
	}
	void ShaderInspector::OnSourceAssetChanged(const std::string& asset)
	{
		if (auto shader = m_Shader.Get())
		{
			shader->SetSourceAsset(asset);
			shader->Save();
		}
	}
}