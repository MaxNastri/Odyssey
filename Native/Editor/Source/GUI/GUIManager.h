#pragma once
#include "EditorEnums.h"
#include "GameObject.h"
#include "InspectorWindow.h"
#include "SceneHierarchyWindow.h"
#include "SceneViewWindow.h"
#include "GameViewWindow.h"
#include "ContentBrowserWindow.h"
#include "RayTracingWindow.h"
#include "EditorMenuBar.h"
#include "EditorActionsBar.h"

namespace Odyssey
{
	struct OnGUIRenderEvent;
	struct SceneLoadedEvent;
	class ImguiPass;

	class GUIManager
	{
	public:
		static void Initialize();

	public:
		static void CreateInspectorWindow(GameObject* gameObject);
		static void CreateSceneHierarchyWindow();
		static void CreateSceneViewWindow();
		static void CreateGameViewWindow();
		static void CreateContentBrowserWindow();
		static void CreateRayTracingWindow();

	public:
		static void Update();
		static void DrawGUI();
		static void SceneLoaded(SceneLoadedEvent* sceneLoadedEvent);
		static void OnGameObjectSelected(int32_t id);
		static void OnSelectionContextChanged(const GUISelection& context);
	public:
		static std::shared_ptr<ImguiPass> GetRenderPass() { return m_GUIPass; }
		static SceneViewWindow* GetSceneViewWindow(uint32_t index) { return sceneViewWindows[index].get(); }
		static GameViewWindow* GetGameViewWindow(uint32_t index) { return gameViewWindows[index].get(); }

	private:
		static void OnFilesChanged(const NotificationSet& notificationSet);
		static void SetDarkThemeColors();

	private:
		inline static EditorMenuBar s_MenuBar;
		inline static EditorActionsBar s_ActionsBar;

		// Windows
		inline static std::vector<std::shared_ptr<InspectorWindow>> inspectorWindows;
		inline static std::vector<std::shared_ptr<SceneHierarchyWindow>> sceneHierarchyWindows;
		inline static std::vector<std::shared_ptr<SceneViewWindow>> sceneViewWindows;
		inline static std::vector<std::shared_ptr<GameViewWindow>> gameViewWindows;
		inline static std::vector<std::shared_ptr<ContentBrowserWindow>> contentBrowserWindows;
		inline static std::vector<std::shared_ptr<RayTracingWindow>> s_RayTracingWindows;
		inline static int32_t selectedObject = -1;
		inline static std::shared_ptr<ImguiPass> m_GUIPass;
	};
}