#pragma once
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
}

namespace Odyssey
{
	struct OnSceneLoaded;
	class Scene;
	class ImguiPass;

	class GUIManager
	{
	public:
		static void Initialize();
		static void CreateInspectorWindow(GameObject* gameObject);
		static void CreateSceneHierarchyWindow();
		static void CreateSceneViewWindow();
		static void CreateContentBrowserWindow();
		static void CreateRayTracingWindow();

	public:
		static void Update();
		static void OnRender(OnGUIRenderEvent* guiRenderEvent);
		static void SceneLoaded(OnSceneLoaded* sceneLoadedEvent);
		static void OnGameObjectSelected(uint32_t id);

	public:
		static std::shared_ptr<ImguiPass> GetRenderPass() { return m_GUIPass; }
		static SceneViewWindow& GetSceneViewWindow(uint32_t index) { return sceneViewWindows[index]; }

	private:
		static void OnFilesChanged(const NotificationSet& notificationSet);

	private:
		inline static EditorMenuBar s_MenuBar;
		inline static std::vector<InspectorWindow> inspectorWindows;
		inline static std::vector<SceneHierarchyWindow> sceneHierarchyWindows;
		inline static std::vector<SceneViewWindow> sceneViewWindows;
		inline static std::vector<ContentBrowserWindow> contentBrowserWindows;
		inline static std::vector<RayTracingWindow> s_RayTracingWindows;
		inline static uint32_t selectedObject = std::numeric_limits<uint32_t>::max();
		inline static std::shared_ptr<ImguiPass> m_GUIPass;
	};
}