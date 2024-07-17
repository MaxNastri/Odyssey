#pragma once
#include "EditorEnums.h"
#include <GameObject.h>
#include "InspectorWindow.h"
#include "SceneHierarchyWindow.h"
#include "SceneViewWindow.h"
#include "ContentBrowserWindow.h"
#include "RayTracingWindow.h"
#include "EditorMenuBar.h"

namespace Odyssey
{
	struct OnGUIRenderEvent;
	struct OnSceneLoaded;
	class ImguiPass;

	class GUIManager
	{
	public:
		GUIManager();

	public:
		void CreateInspectorWindow(GameObject* gameObject);
		void CreateSceneHierarchyWindow();
		void CreateSceneViewWindow();
		void CreateContentBrowserWindow();
		void CreateRayTracingWindow();

	public:
		void Update();
		void DrawGUI();
		void SceneLoaded(OnSceneLoaded* sceneLoadedEvent);
		void OnGameObjectSelected(int32_t id);
		void OnSelectionContextChanged(const GUISelection& context);
		std::function<void(void)> GetDrawGUIListener();
	public:
		std::shared_ptr<ImguiPass> GetRenderPass() { return m_GUIPass; }
		SceneViewWindow& GetSceneViewWindow(uint32_t index) { return sceneViewWindows[index]; }

	private:
		void OnFilesChanged(const NotificationSet& notificationSet);
		void SetDarkThemeColors();

	private:
		EditorMenuBar s_MenuBar;

		// Windows
		std::vector<InspectorWindow> inspectorWindows;
		std::vector<SceneHierarchyWindow> sceneHierarchyWindows;
		std::vector<SceneViewWindow> sceneViewWindows;
		std::vector<ContentBrowserWindow> contentBrowserWindows;
		std::vector<RayTracingWindow> s_RayTracingWindows;
		int32_t selectedObject = -1;
		std::shared_ptr<ImguiPass> m_GUIPass;
	};
}