#include "EditorMenuBar.h"
#include "imgui.h"
#include "FileDialogs.h"
#include "Application.h"
#include "VulkanWindow.h"
#include "SceneManager.h"
#include "GUIManager.h"

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
				void* window = Application::GetRenderer()->GetWindow()->GetNativeWindow();
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
				void* window = Application::GetRenderer()->GetWindow()->GetNativeWindow();
				std::string scenePath = FileDialogs::SaveFile(window, "Odyssey Scene (*.yaml)\0*.yaml\0");

				if (!scenePath.empty())
				{
					SceneManager::SaveActiveSceneTo(scenePath);
				}
			}
            ImGui::EndMenu();
        }
		if (ImGui::BeginMenu("Imgui"))
		{
			if (ImGui::MenuItem("Display Demo Window"))
			{
				m_ShowDemoWindow = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Ray Tracing"))
		{
			GUIManager::CreateRayTracingWindow();
			ImGui::EndMenu();
		}
		if (m_ShowDemoWindow)
		{
			ImGui::ShowDemoWindow(&m_ShowDemoWindow);
		}

		ImGui::EndMainMenuBar();
	}
}