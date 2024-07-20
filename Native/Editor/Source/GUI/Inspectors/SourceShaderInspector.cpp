#include "SourceShaderInspector.h"
#include "imgui.h"
#include "ShaderCompiler.h"

namespace Odyssey
{
	SourceShaderInspector::SourceShaderInspector(const std::filesystem::path& sourcePath)
	{
		m_SourcePath = sourcePath;
		m_SourceDrawer = ReadOnlyStringDrawer("Source Shader", m_SourcePath.string());
		m_CompileDrawer = ReadOnlyStringDrawer("Compiled", std::to_string(m_Compiled));
	}
	void SourceShaderInspector::Draw()
	{
		if (ImGui::BeginTable("MaterialInspector", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
		{
			m_SourceDrawer.Draw();
			m_CompileDrawer.Draw();
			if (ImGui::Button("Compile"))
			{
				ShaderCompiler::Options options;
				options.Optimize = false;
				ShaderCompiler compiler(options);

				auto filename = m_SourcePath.filename().replace_extension("");
				
				// TODO: Need some reflection here
				m_Compiled = compiler.Compile(filename.string(), ShaderType::Vertex, m_SourcePath);
				m_CompileDrawer.SetData(std::to_string(m_Compiled));
			}
			ImGui::EndTable();
		}
	}
}