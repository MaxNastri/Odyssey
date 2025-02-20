#include "EditorMenuBar.h"
#include "imgui.h"
#include "FileDialogs.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "EventSystem.h"
#include "EditorEvents.h"
#include "SceneSettingsWindow.h"
#include "AnimationWindow.h"
#include "PreferencesWindow.h"
#include "TextureGenerationWindow.h"

namespace Odyssey
{
	void EditorMenuBar::Update()
	{

	}

	bool EditorMenuBar::Draw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open"))
				{
					const Path& scenePath = FileDialogs::OpenFile("Odyssey Scene", ".scene");

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
					const Path& scenePath = FileDialogs::SaveFile("Odyssey Scene", ".scene");

					if (!scenePath.empty())
					{
						SceneManager::SaveActiveScene(scenePath);
					}
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Preferences"))
					GUIManager::CreateDockableWindow<PreferencesWindow>();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Create"))
				{

				}
				if (ImGui::MenuItem("Open"))
				{

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
				if (ImGui::MenuItem("Texture Generation Window"))
					GUIManager::CreateDockableWindow<TextureGenerationWindow>();
				if (ImGui::MenuItem("Display Demo Window"))
					m_ShowDemoWindow = true;

				ImGui::EndMenu();
			}

			if (m_ShowDemoWindow)
				ImGui::ShowDemoWindow(&m_ShowDemoWindow);

			ImGui::EndMainMenuBar();
		}

		return false;
	}
}