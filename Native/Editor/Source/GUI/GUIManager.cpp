#include "GUIManager.h"
#include <EventSystem.h>
#include "EditorEvents.h"
#include <Events.h>
#include "SceneManager.h"
#include "Scene.h"
#include "imgui.h"

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
		Scene* scene = SceneManager::GetActiveScene();
		RefHandle<GameObject> gameObject = scene->GetGameObject(id);

		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.SetGameObject(gameObject);
		}
	}
}