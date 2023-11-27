#include "InspectorWindow.h"
#include <ComponentManager.h>
#include <Transform.h>
#include <imgui.h>

namespace Odyssey
{
	InspectorWindow::InspectorWindow(RefHandle<GameObject> gameObject)
	{
		SetGameObject(gameObject);
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

		for (auto& inspector : inspectors)
		{
			inspector->Draw();
		}

		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.Draw();
		}
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void InspectorWindow::SetGameObject(RefHandle<GameObject> gameObject)
	{
		inspectors.clear();
		userScriptInspectors.clear();

		if (ComponentManager::HasComponent<Transform>(gameObject))
		{
			inspectors.push_back(std::make_unique<TransformInspector>(gameObject));
		}

		if (ComponentManager::HasComponent<Camera>(gameObject))
		{
			inspectors.push_back(std::make_unique<CameraInspector>(gameObject));
		}

		std::vector<std::pair<std::string, UserScript*>> userScripts = ComponentManager::GetAllUserScripts(gameObject);

		for (auto& [userScriptClassName, userScript] : userScripts)
		{
			userScriptInspectors.push_back(UserScriptInspector(gameObject, userScript, userScriptClassName));
		}
	}

	void InspectorWindow::RefreshUserScripts()
	{
		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.UpdateFields();
		}
	}
}