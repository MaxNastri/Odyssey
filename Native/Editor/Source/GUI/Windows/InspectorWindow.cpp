#include "InspectorWindow.h"
#include "imgui.h"
#include "Inspector.h"
#include "GameObjectInspector.h"
#include "MaterialInspector.h"
#include "MeshRendererInspector.h"
#include "SourceShaderInspector.h"
#include "ShaderInspector.h"

namespace Odyssey
{
	InspectorWindow::InspectorWindow(std::shared_ptr<Inspector> inspector)
	{
		m_Inspector = inspector;
	}

	void InspectorWindow::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Inspector", &open))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		if (m_Inspector)
			m_Inspector->Draw();
		
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void InspectorWindow::OnSelectionContextChanged(const GUISelection& context)
	{
		// TODO: Convert this to use ClassName::Type
		// Components/GameObject are setup like this but not assets
		if (context.Type == GameObject::Type)
			m_Inspector = std::make_shared<GameObjectInspector>(context.ID);
		else if (context.Type == "Material")
			m_Inspector = std::make_shared<MaterialInspector>(context.GUID);
		else if (context.Type == "SourceShader")
			m_Inspector = std::make_shared<SourceShaderInspector>(context.GUID);
		else if (context.Type == "Shader")
			m_Inspector = std::make_shared<ShaderInspector>(context.GUID);
	}
}