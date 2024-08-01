#include "GUIManager.h"
#include "EventSystem.h"
#include "EditorEvents.h"
#include <Events.h>
#include "SceneManager.h"
#include "Scene.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Camera.h"
#include "RenderPasses.h"
#include "Editor.h"
#include "RayTracingWindow.h"
#include "GameObjectInspector.h"
#include "imgui_internal.h"

namespace Odyssey
{
	void GUIManager::Initialize()
	{
		GUIManager::SetDarkThemeColors();
	}

	void GUIManager::CreateInspectorWindow(GameObject* gameObject)
	{
		s_Windows.push_back(std::make_shared<InspectorWindow>(std::make_shared<GameObjectInspector>(gameObject)));
	}

	void GUIManager::CreateSceneHierarchyWindow()
	{
		s_Windows.push_back(std::make_shared<SceneHierarchyWindow>());
	}

	void GUIManager::CreateSceneViewWindow()
	{
		auto sceneView = std::make_shared<SceneViewWindow>();
		s_SceneViews.push_back(sceneView);
		s_Windows.push_back(sceneView);
	}

	void GUIManager::CreateGameViewWindow()
	{
		auto gameView = std::make_shared<GameViewWindow>();
		s_GameViews.push_back(gameView);
		s_Windows.push_back(gameView);
	}

	void GUIManager::CreateContentBrowserWindow()
	{
		s_Windows.push_back(std::make_shared<ContentBrowserWindow>());
	}

	void GUIManager::Update()
	{
		s_MenuBar.Update();
		s_ActionsBar.Update();

		for (auto& window : s_Windows)
		{
			window->Update();
		}
	}

	void GUIManager::DrawGUI()
	{
		uint32_t flags = 0;
		flags |= ImGuiDockNodeFlags_NoWindowMenuButton;
		flags |= ImGuiDockNodeFlags_NoCloseButton;

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), flags);

		s_MenuBar.Draw();
		s_ActionsBar.Draw();

		for (auto& window : s_Windows)
		{
			window->Draw();
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