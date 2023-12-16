#include "GUIManager.h"
#include <EventSystem.h>
#include "EditorEvents.h"
#include <Events.h>
#include "SceneManager.h"
#include "Scene.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Camera.h"
#include "RenderPasses.h"
#include "Application.h"
#include "RayTracingWindow.h"

namespace Odyssey
{
	void GUIManager::Initialize()
	{
		EventSystem::Listen<OnSceneLoaded>(GUIManager::SceneLoaded);
		EventSystem::Listen<OnGUIRenderEvent>(GUIManager::OnRender);
		FileManager::AddFilesChangedCallback(GUIManager::OnFilesChanged);

		m_GUIPass = std::make_shared<ImguiPass>();
		m_GUIPass->SetLayouts(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		m_GUIPass->SetImguiState(Application::GetRenderer()->GetImGui());
	}

	void GUIManager::CreateInspectorWindow(GameObject* gameObject)
	{
		inspectorWindows.push_back(InspectorWindow(gameObject));
	}

	void GUIManager::CreateSceneHierarchyWindow()
	{
		sceneHierarchyWindows.push_back(SceneHierarchyWindow());
		sceneHierarchyWindows[sceneHierarchyWindows.size() - 1].OnGameObjectSelected(OnGameObjectSelected);
	}

	void GUIManager::CreateSceneViewWindow()
	{
		sceneViewWindows.push_back(SceneViewWindow());
	}

	void GUIManager::CreateContentBrowserWindow()
	{
		contentBrowserWindows.push_back(ContentBrowserWindow());
	}

	void GUIManager::CreateRayTracingWindow()
	{
		s_RayTracingWindows.push_back(RayTracingWindow());
	}

	void GUIManager::Update()
	{
		s_MenuBar.Update();

		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.Update();
		}

		for (auto& sceneHierarchyWindow : sceneHierarchyWindows)
		{
			sceneHierarchyWindow.Update();
		}

		for (auto& sceneViewWindow : sceneViewWindows)
		{
			sceneViewWindow.Update();
		}

		for (auto& contentBrowserWindow : contentBrowserWindows)
		{
			contentBrowserWindow.Update();
		}

		for (auto& rayTracingWindow : s_RayTracingWindows)
		{
			rayTracingWindow.Update();
		}
	}

	void GUIManager::OnRender(OnGUIRenderEvent* guiRenderEvent)
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		s_MenuBar.Draw();

		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.Draw();
		}

		for (auto& sceneHierarchyWindow : sceneHierarchyWindows)
		{
			sceneHierarchyWindow.Draw();
		}

		for (auto& sceneViewWindow : sceneViewWindows)
		{
			sceneViewWindow.Draw();
		}

		for (auto& contentBrowserWindow : contentBrowserWindows)
		{
			contentBrowserWindow.Draw();
		}

		for (auto& rayTracingWindow : s_RayTracingWindows)
		{
			rayTracingWindow.Draw();
		}
	}

	void GUIManager::SceneLoaded(OnSceneLoaded* sceneLoadedEvent)
	{
		for (auto& sceneHierarchyWindow : sceneHierarchyWindows)
		{
			sceneHierarchyWindow.OnSceneChanged();
		}

		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.RefreshUserScripts();
		}
	}

	void GUIManager::OnGameObjectSelected(int32_t id)
	{
		selectedObject = id;

		Scene* scene = SceneManager::GetActiveScene();
		GameObject* gameObject = scene->GetGameObject(id);

		for (auto& inspectorWindow : inspectorWindows)
		{
			inspectorWindow.SetGameObject(gameObject);
		}

		for (auto& sceneViewWindow : sceneViewWindows)
		{
			sceneViewWindow.SetSelectedIndex(gameObject->id);
		}
	}

	void GUIManager::OnFilesChanged(const NotificationSet& notificationSet)
	{
		for (auto& contentBrowserWindow : contentBrowserWindows)
		{
			contentBrowserWindow.UpdatePaths();
		}
	}
}