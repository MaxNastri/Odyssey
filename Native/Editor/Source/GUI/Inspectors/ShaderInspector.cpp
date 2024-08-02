#include "ShaderInspector.h"
#include "AssetManager.h"
#include "Shader.h"

namespace Odyssey
{
	ShaderInspector::ShaderInspector(GUID guid)
	{
		m_Shader = AssetManager::LoadShaderByGUID(guid);

		if (auto shader = m_Shader.Get())
		{
			m_GUIDDrawer = StringDrawer("GUID", shader->GetGUID().String(), nullptr, true);
			m_NameDrawer = StringDrawer("Name", shader->GetName(), 
				[this](const std::string& name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", shader->GetType(), nullptr, true);
			m_ShaderCodeDrawer = StringDrawer("Shader Code", shader->GetShaderCodeGUID(), nullptr, true);
			m_ShaderTypeDrawer = IntDrawer<uint32_t>("Shader Type", (uint32_t)shader->GetShaderType(), nullptr, true);
			m_SourceShaderDrawer = AssetFieldDrawer("Source Asset", shader->GetSoureAsset(), "SourceShader",
				[this](GUID sourceGUID) { OnSourceAssetChanged(sourceGUID); });
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
	void ShaderInspector::OnSourceAssetChanged(GUID sourceGUID)
	{
		if (auto shader = m_Shader.Get())
		{
			shader->SetSourceAsset(sourceGUID);
			shader->Save();
		}
	}
}