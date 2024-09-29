#include "ShaderInspector.h"
#include "AssetManager.h"
#include "Shader.h"
#include "SourceShader.h"

namespace Odyssey
{
	ShaderInspector::ShaderInspector(GUID guid)
	{
		if (m_Shader = AssetManager::LoadAsset<Shader>(guid))
		{
			m_GUIDDrawer = StringDrawer("GUID", m_Shader->GetGUID().String(), nullptr, true);
			m_NameDrawer = StringDrawer("Name", m_Shader->GetName(),
				[this](const std::string& name) { OnNameChanged(name); });
			m_SourceShaderDrawer = AssetFieldDrawer("Source Asset", m_Shader->GetSoureAsset(), SourceShader::Type,
				[this](GUID sourceGUID) { OnSourceAssetChanged(sourceGUID); });
		}
	}

	void ShaderInspector::Draw()
	{
		m_GUIDDrawer.Draw();
		m_NameDrawer.Draw();
		m_SourceShaderDrawer.Draw();

		if (ImGui::Button("Compile"))
			m_Shader->Recompile();
	}
	void ShaderInspector::OnNameChanged(const std::string& name)
	{
		if (m_Shader)
		{
			m_Shader->SetName(name);
			m_Shader->Save();
		}
	}
	void ShaderInspector::OnSourceAssetChanged(GUID sourceGUID)
	{
		if (m_Shader)
		{
			m_Shader->SetSourceAsset(sourceGUID);
			m_Shader->Save();
		}
	}
}