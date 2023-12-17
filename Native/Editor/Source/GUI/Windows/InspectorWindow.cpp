#include "InspectorWindow.h"
#include "Inspector.h"
#include "GameObjectInspector.h"
#include "MaterialInspector.h"
#include "MeshRendererInspector.h"
#include "imgui.h"

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
		switch (context.Type)
		{
			case GUISelection::SelectionType::None:
				break;
			case GUISelection::SelectionType::GameObject:
				m_Inspector = std::make_shared<GameObjectInspector>(context.ID);
				break;
			case GUISelection::SelectionType::Material:
				m_Inspector = std::make_shared<MaterialInspector>(context.guid);
				break;
			case GUISelection::SelectionType::Mesh:
				break;
			case GUISelection::SelectionType::Shader:
				break;
		}
	}
}