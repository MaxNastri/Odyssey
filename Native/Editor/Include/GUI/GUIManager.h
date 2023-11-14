#pragma once
#include <GameObject.h>
#include "InspectorWindow.h"

namespace Odyssey::Graphics
{
	struct OnGUIRenderEvent;
}

namespace Odyssey::Editor
{
	struct OnSceneLoaded;

	class GUIManager
	{
	public:
		static void ListenForEvents();
		static void CreateInspectorWindow(GameObject gameObject);

	public:
		static void OnRender(Graphics::OnGUIRenderEvent* guiRenderEvent);
		static void SceneLoaded(OnSceneLoaded* sceneLoadedEvent);

	private:
		static std::vector<InspectorWindow> inspectorWindows;
	};
}