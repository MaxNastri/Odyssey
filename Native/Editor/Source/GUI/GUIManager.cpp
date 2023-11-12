#include "GUIManager.h"
#include <EventSystem.h>
#include "EditorEvents.h"
#include <GraphicsEvents.h>

namespace Odyssey::Editor
{
	std::vector<InspectorWindow> GUIManager::inspectorWindows;

	void GUIManager::ListenForEvents()
	{
		Framework::EventSystem::Listen<OnSceneLoaded>(GUIManager::SceneLoaded);
		Framework::EventSystem::Listen<Graphics::OnGUIRenderEvent>(GUIManager::OnRender);
	}

	void GUIManager::CreateInspectorWindow(Entities::GameObject gameObject)
	{
		inspectorWindows.push_back(InspectorWindow(gameObject));
	}

	void GUIManager::OnRender(Graphics::OnGUIRenderEvent* guiRenderEvent)
	{
		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.Draw();
		}
	}

	void GUIManager::SceneLoaded(OnSceneLoaded* sceneLoadedEvent)
	{
		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.RefreshUserScripts();
		}
	}
}