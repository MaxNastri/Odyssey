#include "InspectorWindow.h"
#include <ComponentManager.h>
#include <Transform.h>
#include <imgui.h>

namespace Odyssey::Editor
{
	InspectorWindow::InspectorWindow(GameObject gameObject)
	{
		if (ComponentManager::HasComponent<Transform>(gameObject))
		{
			transformInspector = TransformInspector(gameObject);
		}

		std::vector<std::pair<std::string, UserScript*>> userScripts = ComponentManager::GetAllUserScripts(gameObject);

		for (auto& [userScriptClassName, userScript] : userScripts)
		{
			userScriptInspectors.push_back(UserScriptInspector(gameObject, userScript, userScriptClassName));
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
		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.Draw();
		}
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void InspectorWindow::RefreshUserScripts()
	{
		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.UpdateFields();
		}
	}
}