#include "EditorMenuBar.h"
#include "imgui.h"
#include "FileDialogs.h"
#include "VulkanWindow.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "EventSystem.h"
#include "EditorEvents.h"
#include "Renderer.h"
#include "SceneSettingsWindow.h"
#include "AnimationWindow.h"

namespace Odyssey
{
	void EditorMenuBar::Update()
	{

	}

	void EditorMenuBar::Draw()
	{
		if (!ImGui::BeginMainMenuBar())
		{
			ImGui::EndMainMenuBar();
			return;
		}
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				void* window = Renderer::GetWindow()->GetNativeWindow();
				std::string scenePath = FileDialogs::OpenFile(window, "Odyssey Scene (*.yaml)\0*.yaml\0");

				if (!scenePath.empty())
				{
					SceneManager::LoadScene(scenePath);
				}
			}
			if (ImGui::MenuItem("Save"))
			{
				SceneManager::SaveActiveScene();
			}
			if (ImGui::MenuItem("Save As..."))
			{
				void* window = Renderer::GetWindow()->GetNativeWindow();
				std::string scenePath = FileDialogs::SaveFile(window, "Odyssey Scene (*.yaml)\0*.yaml\0");

				if (!scenePath.empty())
				{
					SceneManager::SaveActiveScene(scenePath);
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Animation Window"))
				GUIManager::CreateDockableWindow<AnimationWindow>();
			if (ImGui::MenuItem("Content Browser"))
				GUIManager::CreateDockableWindow<ContentBrowserWindow>();
			if (ImGui::MenuItem("Game View"))
				GUIManager::CreateDockableWindow<GameViewWindow>();
			if (ImGui::MenuItem("Inspector Window"))
				GUIManager::CreateDockableWindow<InspectorWindow>();
			if (ImGui::MenuItem("Scene Hierarchy"))
				GUIManager::CreateDockableWindow<SceneHierarchyWindow>();
			if (ImGui::MenuItem("Scene Settings"))
				GUIManager::CreateDockableWindow<SceneSettingsWindow>();
			if (ImGui::MenuItem("Scene View"))
				GUIManager::CreateDockableWindow<SceneViewWindow>();

			if (ImGui::MenuItem("Display Demo Window"))
				m_ShowDemoWindow = true;

			ImGui::EndMenu();
		}

		if (m_ShowDemoWindow)
			ImGui::ShowDemoWindow(&m_ShowDemoWindow);

		ImGui::EndMainMenuBar();
	}
}