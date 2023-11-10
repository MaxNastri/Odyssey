#include "InspectorWindow.h"
#include <ComponentManager.h>
#include <Transform.h>
#include <imgui.h>

namespace Odyssey::Editor
{
	InspectorWindow::InspectorWindow(Entities::GameObject gameObject)
	{
		using namespace Entities;

		if (ComponentManager::HasComponent<Transform>(gameObject))
		{
			transformInspector = TransformInspector(gameObject);
			transformInspector.RegisterCallbacks();
		}
	}

	void InspectorWindow::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Inspector Window", &open))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		transformInspector.Draw();

		ImGui::PopStyleVar();
		ImGui::End();
	}
}