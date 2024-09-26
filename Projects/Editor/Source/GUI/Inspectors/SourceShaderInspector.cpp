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
			m_ShaderNameDrawer = StringDrawer("Shader Name", m_Shader->GetName(), nullptr, true);
			m_ShaderLanguageDrawer = StringDrawer("Shader Language", m_Shader->GetShaderLanguage(), nullptr, true);
			m_CompiledDrawer = BoolDrawer("Compiled", m_Shader->IsCompiled(), nullptr, true);
			m_DstAssetPathDrawer = StringDrawer("Destination Asset Path", m_DstAssetPath,
				[this](const std::string& assetPath) { OnDstAssetPathChanged(assetPath); });

			m_ShaderNameDrawer.SetLabelWidth(0.5f);
			m_ShaderLanguageDrawer.SetLabelWidth(0.5f);
			m_CompiledDrawer.SetLabelWidth(0.5f);
			m_DstAssetPathDrawer.SetLabelWidth(0.7f);
		}
	}
	void SourceShaderInspector::Draw()
	{
		m_ShaderNameDrawer.Draw();
		m_ShaderLanguageDrawer.Draw();
		m_CompiledDrawer.Draw();
		m_DstAssetPathDrawer.Draw();

		if (ImGui::Button("Compile"))
		{
			m_CompiledDrawer.SetData(m_Shader->Compile());
		}
		if (ImGui::Button("Create Shader"))
		{
			if (!m_DstAssetPath.empty())
				AssetManager::CreateAsset<Shader>(Project::GetActiveAssetsDirectory() / m_DstAssetPath, m_Shader);
		}
	}
}