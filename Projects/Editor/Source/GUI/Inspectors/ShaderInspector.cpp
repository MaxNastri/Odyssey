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
			m_GUIDDrawer = StringDrawer("GUID", m_Shader->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Shader->GetName(), false,
				[this](std::string_view name) { OnNameChanged(name); });
			m_SourceShaderDrawer = AssetFieldDrawer("Source Asset", m_Shader->GetSourceAsset(), SourceShader::Type,
				[this](GUID sourceGUID) { OnSourceAssetChanged(sourceGUID); });
		}
	}

	bool ShaderInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();
		modified |= m_NameDrawer.Draw();
		modified |= m_SourceShaderDrawer.Draw();

		if (ImGui::Button("Compile"))
			m_Shader->Recompile();

		return modified;
	}
	void ShaderInspector::OnNameChanged(std::string_view name)
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