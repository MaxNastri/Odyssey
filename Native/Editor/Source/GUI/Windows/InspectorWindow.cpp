#include "InspectorWindow.h"
#include <ComponentManager.h>
#include <Transform.h>
#include <imgui.h>

namespace Odyssey
{
	InspectorWindow::InspectorWindow(GameObject* gameObject)
	{
		SetGameObject(gameObject);
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

	void InspectorWindow::SetGameObject(GameObject* gameObject)
	{
		inspectors.clear();
		userScriptInspectors.clear();

		if (gameObject)
		{
			if (ComponentManager::HasComponent<Transform>(gameObject->id))
			{
				inspectors.push_back(std::make_unique<TransformInspector>(gameObject));
			}

			if (ComponentManager::HasComponent<Camera>(gameObject->id))
			{
				inspectors.push_back(std::make_unique<CameraInspector>(gameObject));
			}

			std::vector<std::pair<std::string, UserScript*>> userScripts = ComponentManager::GetAllUserScripts(gameObject->id);

			for (auto& [userScriptClassName, userScript] : userScripts)
			{
				userScriptInspectors.push_back(UserScriptInspector(gameObject, userScript, userScriptClassName));
			}
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