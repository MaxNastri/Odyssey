#include "InspectorWindow.h"
#include "ComponentManager.h"
#include <Transform.h>
#include <imgui.h>
#include "MeshRendererInspector.h"

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
			if (gameObject->HasComponent<Transform>())
			{
				inspectors.push_back(std::make_unique<TransformInspector>(gameObject));
			}

			if (gameObject->HasComponent<Camera>())
			{
				inspectors.push_back(std::make_unique<CameraInspector>(gameObject));
			}

			if (gameObject->HasComponent<MeshRenderer>())
			{
				inspectors.push_back(std::make_unique<MeshRendererInspector>(gameObject));
			}

			std::vector<UserScript*> userScripts = ComponentManager::GetAllUserScripts(gameObject->id);

			for (UserScript* userScript : userScripts)
			{
				userScriptInspectors.push_back(UserScriptInspector(gameObject, userScript, userScript->GetManagedTypeName()));
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