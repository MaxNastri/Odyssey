#pragma once
#include <GameObject.h>
#include "InspectorWindow.h"
#include "SceneHierarchyWindow.h"

namespace Odyssey
{
	struct OnGUIRenderEvent;
}

namespace Odyssey
{
	struct OnSceneLoaded;
	class Scene;

	class GUIManager
	{
	public:
		static void ListenForEvents();
		static void CreateInspectorWindow(RefHandle<GameObject> gameObject);
		static void CreateSceneHierarchyWindow(std::shared_ptr<Scene> scene);

	public:
		static void OnRender(OnGUIRenderEvent* guiRenderEvent);
		static void SceneLoaded(OnSceneLoaded* sceneLoadedEvent);
		static void OnGameObjectSelected(uint32_t id);

	private:
		inline static std::vector<InspectorWindow> inspectorWindows;
		inline static std::vector<SceneHierarchyWindow> sceneHierarchyWindows;
	};
}