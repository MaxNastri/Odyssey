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
#include "GameObjectInspector.h"

namespace Odyssey
{
	void GUIManager::Initialize()
	{
		EventSystem::Listen<OnSceneLoaded>(SceneLoaded);
		FileManager::AddFilesChangedCallback(OnFilesChanged);

		m_GUIPass = std::make_shared<ImguiPass>();
		m_GUIPass->SetLayouts(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		m_GUIPass->SetImguiState(Application::GetRenderer()->GetImGui());

		GUIManager::SetDarkThemeColors();
	}

	void GUIManager::CreateInspectorWindow(GameObject* gameObject)
	{
		inspectorWindows.push_back(InspectorWindow(std::make_shared<GameObjectInspector>(gameObject)));
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

	void GUIManager::DrawGUI()
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
			inspectorWindow.Reload();
		}
	}

	void GUIManager::OnGameObjectSelected(int32_t id)
	{
		selectedObject = id;

		Scene* scene = SceneManager::GetActiveScene();
		GameObject* gameObject = scene->GetGameObject(id);

		for (auto& sceneViewWindow : sceneViewWindows)
		{
			sceneViewWindow.SetSelectedIndex(gameObject->id);
		}
	}

	void GUIManager::OnSelectionContextChanged(const GUISelection& context)
	{
		// foreach window, invoke event
		for (auto& window : inspectorWindows)
		{
			window.OnSelectionContextChanged(context);
		}
	}

	void GUIManager::OnFilesChanged(const NotificationSet& notificationSet)
	{
		for (auto& contentBrowserWindow : contentBrowserWindows)
		{
			// TODO: Move the listener into the content browser
			contentBrowserWindow.UpdatePaths();
		}
	}
	void GUIManager::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}
}