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
		static void CreateInspectorWindow();
		static void CreateSceneHierarchyWindow();
		static void CreateSceneViewWindow();
		static void CreateGameViewWindow();
		static void CreateContentBrowserWindow();

	public:
		static void Update();
		static void DrawGUI();

	public:
		static std::shared_ptr<ImguiPass> GetRenderPass() { return m_GUIPass; }
		static std::shared_ptr<SceneViewWindow> GetSceneViewWindow(uint32_t index) { return s_SceneViews[index]; }
		static std::shared_ptr<GameViewWindow> GetGameViewWindow(uint32_t index) { return s_GameViews[index]; }

	private:
		static void SetDarkThemeColors();

	private:
		inline static EditorMenuBar s_MenuBar;
		inline static EditorActionsBar s_ActionsBar;

		// Windows
		inline static std::vector<std::shared_ptr<DockableWindow>> s_Windows;
		inline static std::vector<std::shared_ptr<SceneViewWindow>> s_SceneViews;
		inline static std::vector<std::shared_ptr<GameViewWindow>> s_GameViews;
		inline static int32_t selectedObject = -1;
		inline static std::shared_ptr<ImguiPass> m_GUIPass;
	};
}