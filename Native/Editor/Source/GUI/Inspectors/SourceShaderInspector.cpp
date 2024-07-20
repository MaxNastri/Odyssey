#include "SourceShaderInspector.h"
#include "SourceShader.h"
#include "AssetManager.h"
#include "imgui.h"
#include "ShaderCompiler.h"

namespace Odyssey
{
	SourceShaderInspector::SourceShaderInspector(const std::string& guid)
	{
		m_Shader = AssetManager::LoadSourceShader(guid);

		m_ShaderNameDrawer = StringDrawer("Shader Name", m_Shader.Get()->GetName(), nullptr, true);
		m_ShaderNameDrawer.SetLabelWidth(0.5f);
		m_ShaderLanguageDrawer = StringDrawer("Shader Language", m_Shader.Get()->GetShaderLanguage(), nullptr, true);
		m_ShaderLanguageDrawer.SetLabelWidth(0.5f);
		m_CompiledDrawer = BoolDrawer("Compiled", m_Shader.Get()->IsCompiled(), nullptr, true);
		m_CompiledDrawer.SetLabelWidth(0.5f);
	}
	void SourceShaderInspector::Draw()
	{
		m_ShaderNameDrawer.Draw();
		m_ShaderLanguageDrawer.Draw();
		m_CompiledDrawer.Draw();
		if (ImGui::Button("Compile"))
		{
			m_CompiledDrawer.SetData(m_Shader.Get()->Compile());
		}
	}
}