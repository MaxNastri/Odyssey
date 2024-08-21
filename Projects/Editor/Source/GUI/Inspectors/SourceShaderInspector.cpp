#include "SourceShaderInspector.h"
#include "SourceShader.h"
#include "AssetManager.h"
#include "imgui.h"
#include "ShaderCompiler.h"
#include "Shader.h"
#include "Project.h"

namespace Odyssey
{
	SourceShaderInspector::SourceShaderInspector(GUID guid)
	{
		m_Shader = AssetManager::LoadSourceShader(guid);

		m_ShaderNameDrawer = StringDrawer("Shader Name", m_Shader.Get()->GetName(), nullptr, true);
		
		m_ShaderLanguageDrawer = StringDrawer("Shader Language", m_Shader.Get()->GetShaderLanguage(), nullptr, true);

		m_ShaderTypeDrawer = IntDrawer<uint32_t>("Shader Type", 0,
			[this](uint32_t shaderType) { m_Shader.Get()->SetShaderType((ShaderType)shaderType); });
		
		m_CompiledDrawer = BoolDrawer("Compiled", m_Shader.Get()->IsCompiled(), nullptr, true);

		m_DstAssetPathDrawer = StringDrawer("Destination Asset Path", m_DstAssetPath,
			[this](const std::string& assetPath) { OnDstAssetPathChanged(assetPath); });
		
		m_ShaderNameDrawer.SetLabelWidth(0.5f);
		m_ShaderLanguageDrawer.SetLabelWidth(0.5f);
		m_ShaderTypeDrawer.SetLabelWidth(0.5f);
		m_CompiledDrawer.SetLabelWidth(0.5f);
		m_DstAssetPathDrawer.SetLabelWidth(0.7f);
	}
	void SourceShaderInspector::Draw()
	{
		m_ShaderNameDrawer.Draw();
		m_ShaderLanguageDrawer.Draw();
		m_ShaderTypeDrawer.Draw();
		m_CompiledDrawer.Draw();
		m_DstAssetPathDrawer.Draw();
		if (ImGui::Button("Compile"))
		{
			m_CompiledDrawer.SetData(m_Shader.Get()->Compile());
		}
		if (ImGui::Button("Create Shader"))
		{
			if (!m_DstAssetPath.empty())
				AssetManager::CreateShader(Project::GetActiveAssetsDirectory() / m_DstAssetPath, m_Shader);
		}
	}
}