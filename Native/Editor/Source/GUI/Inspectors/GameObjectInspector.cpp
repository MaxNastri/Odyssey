#include "GameObjectInspector.h"
#include "MeshRendererInspector.h"
#include "SceneManager.h"
#include "Scene.h"
#include "imgui.h"

namespace Odyssey
{
	GameObjectInspector::GameObjectInspector(GameObject* gameObject)
	{
		SetGameObject(gameObject);
	}

	GameObjectInspector::GameObjectInspector(int32_t gameObjectID)
	{
		if (Scene* scene = SceneManager::GetActiveScene())
		{
			if (GameObject* gameObject = scene->GetGameObject(gameObjectID))
			{
				SetGameObject(gameObject);
			}
		}
	}

	void GameObjectInspector::Draw()
	{
		if (m_Inspectors.size() == 0)
			return;

		if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_NameDrawer.Draw();
		}

		ImGui::Spacing();

		for (auto& inspector : m_Inspectors)
		{
			inspector->Draw();
		}

		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.Draw();
		}
	}

	void GameObjectInspector::SetGameObject(GameObject* gameObject)
	{
		m_Inspectors.clear();
		userScriptInspectors.clear();

		if (gameObject)
		{
			m_NameDrawer = StringDrawer("Name", gameObject->name,
				[gameObject](std::string& name) { gameObject->name = name; });

			if (gameObject->HasComponent<Transform>())
			{
				m_Inspectors.push_back(std::make_unique<TransformInspector>(gameObject));
			}

			if (gameObject->HasComponent<Camera>())
			{
				m_Inspectors.push_back(std::make_unique<CameraInspector>(gameObject));
			}

			if (gameObject->HasComponent<MeshRenderer>())
			{
				m_Inspectors.push_back(std::make_unique<MeshRendererInspector>(gameObject));
			}

			std::vector<UserScript*> userScripts = gameObject->GetUserScripts();

			for (UserScript* userScript : userScripts)
			{
				userScriptInspectors.push_back(UserScriptInspector(gameObject, userScript, userScript->GetManagedTypeName()));
			}
		}
	}

	void GameObjectInspector::RefreshUserScripts()
	{
		for (auto& userScriptInspector : userScriptInspectors)
		{
			userScriptInspector.UpdateFields();
		}
	}
}