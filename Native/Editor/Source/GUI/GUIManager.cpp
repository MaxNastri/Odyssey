#include "GUIManager.h"
#include <EventSystem.h>
#include "EditorEvents.h"
#include <Events.h>
#include "SceneManager.h"
#include "Scene.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Camera.h"
#include "ComponentManager.h"

namespace Odyssey
{
	void GUIManager::ListenForEvents()
	{
		EventSystem::Listen<OnSceneLoaded>(GUIManager::SceneLoaded);
		EventSystem::Listen<OnGUIRenderEvent>(GUIManager::OnRender);
	}

	void GUIManager::CreateInspectorWindow(RefHandle<GameObject> gameObject)
	{
		inspectorWindows.push_back(InspectorWindow(gameObject));
	}

	void GUIManager::CreateSceneHierarchyWindow(std::shared_ptr<Scene> scene)
	{
		sceneHierarchyWindows.push_back(SceneHierarchyWindow(scene));
		sceneHierarchyWindows[sceneHierarchyWindows.size() - 1].OnGameObjectSelected(OnGameObjectSelected);
	}

	void GUIManager::OnRender(OnGUIRenderEvent* guiRenderEvent)
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		RenderGizmos();

		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.Draw();
		}

		for (auto& sceneHierarchyWindow : sceneHierarchyWindows)
		{
			sceneHierarchyWindow.Draw();
		}
	}

	void GUIManager::SceneLoaded(OnSceneLoaded* sceneLoadedEvent)
	{
		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.RefreshUserScripts();
		}
	}

	void GUIManager::OnGameObjectSelected(uint32_t id)
	{
		selectedObject = id;

		Scene* scene = SceneManager::GetActiveScene();
		RefHandle<GameObject> gameObject = scene->GetGameObject(id);

		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.SetGameObject(gameObject);
		}
	}
	void GUIManager::RenderGizmos()
	{
		Scene* scene = SceneManager::GetActiveScene();

		if (selectedObject != std::numeric_limits<uint32_t>::max())
		{
			Transform* component = ComponentManager::GetComponent<Transform>(selectedObject);
			glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 proj = glm::perspectiveLH(glm::radians(90.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
			proj[1][1] = proj[1][1];

			glm::mat4 transform = component->GetWorldMatrix();

			ImGuizmo::SetRect(500, 500, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj),
				ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform));

			if (ImGuizmo::IsUsing())
			{
				component->SetPosition(glm::vec3(transform[3]));
			}
		}
	}
}