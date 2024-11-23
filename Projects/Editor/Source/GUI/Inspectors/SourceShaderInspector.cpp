#include "SourceShaderInspector.h"
#include "SourceShader.h"
#include "AssetManager.h"
#include "imgui.h"
#include "ShaderCompiler.h"
#include "Shader.h"
#include "Project.h"
#include "Shader.h"

namespace Odyssey
{
	SourceShaderInspector::SourceShaderInspector(GUID guid)
	{
		if (m_Shader = AssetManager::LoadSourceAsset<SourceShader>(guid))
		{
			m_ShaderNameDrawer = StringDrawer("Shader Name", m_Shader->GetName(), true);
			m_ShaderLanguageDrawer = StringDrawer("Shader Language", m_Shader->GetShaderLanguage(), true);
			m_CompiledDrawer = BoolDrawer("Compiled", m_Shader->IsCompiled(), true);
			m_DstAssetPathDrawer = StringDrawer("Destination Asset Path", m_DstAssetPath, false,
				[this](std::string_view assetPath) { OnDstAssetPathChanged(assetPath); });

			m_ShaderNameDrawer.SetLabelWidth(0.5f);
			m_ShaderLanguageDrawer.SetLabelWidth(0.5f);
			m_CompiledDrawer.SetLabelWidth(0.5f);
			m_DstAssetPathDrawer.SetLabelWidth(0.7f);
		}
	}

	bool SourceShaderInspector::Draw()
	{
		bool modified = false;
		modified |= m_ShaderNameDrawer.Draw();
		modified |= m_ShaderLanguageDrawer.Draw();
		modified |= m_CompiledDrawer.Draw();
		modified |= m_DstAssetPathDrawer.Draw();

		if (ImGui::Button("Compile"))
		{
			m_CompiledDrawer.SetValue(m_Shader->Compile());
		}
		if (ImGui::Button("Create Shader"))
		{
			if (!m_DstAssetPath.empty())
				AssetManager::CreateAsset<Shader>(Project::GetActiveAssetsDirectory() / m_DstAssetPath, m_Shader);
		}

		return modified;
	}
}