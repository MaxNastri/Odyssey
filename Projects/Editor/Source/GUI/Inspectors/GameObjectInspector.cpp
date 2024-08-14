#include "GameObjectInspector.h"
#include "MeshRendererInspector.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "ScriptComponent.h"
#include "imgui.h"

namespace Odyssey
{
	GameObjectInspector::GameObjectInspector(GUID guid)
	{
		GameObject gameObject = SceneManager::GetActiveScene()->GetGameObject(guid);
		SetGameObject(gameObject);
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

	void GameObjectInspector::SetGameObject(GameObject& gameObject)
	{
		m_Inspectors.clear();
		userScriptInspectors.clear();

		m_NameDrawer = StringDrawer("Name", gameObject.GetName(),
			[&gameObject](std::string& name) { gameObject.SetName(name); });

		if (gameObject.HasComponent<Transform>())
		{
			m_Inspectors.push_back(std::make_unique<TransformInspector>(gameObject));
		}

		if (gameObject.HasComponent<Camera>())
		{
			m_Inspectors.push_back(std::make_unique<CameraInspector>(gameObject));
		}

		if (gameObject.HasComponent<MeshRenderer>())
		{
			m_Inspectors.push_back(std::make_unique<MeshRendererInspector>(gameObject));
		}

		if (gameObject.HasComponent<ScriptComponent>())
		{
			m_Inspectors.push_back(std::make_unique<UserScriptInspector>(gameObject));
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