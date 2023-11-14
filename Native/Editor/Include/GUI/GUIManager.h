#pragma once
#include <GameObject.h>
#include "InspectorWindow.h"

namespace Odyssey
{
	struct OnGUIRenderEvent;
}

namespace Odyssey
{
	struct OnSceneLoaded;

	class GUIManager
	{
	public:
		static void ListenForEvents();
		static void CreateInspectorWindow(GameObject gameObject);

	public:
		static void OnRender(OnGUIRenderEvent* guiRenderEvent);
		static void SceneLoaded(OnSceneLoaded* sceneLoadedEvent);

	private:
		static std::vector<InspectorWindow> inspectorWindows;
	};
}