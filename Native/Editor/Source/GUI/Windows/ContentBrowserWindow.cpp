#include "ContentBrowserWindow.h"
#include "Input.h"
#include "imgui.h"
#include "SceneManager.h"
#include "AssetManager.h"

namespace Odyssey
{
	static std::filesystem::path s_AssetsPath = "Assets";

	ContentBrowserWindow::ContentBrowserWindow()
	{
		m_WindowPos = glm::vec2(0, 0);
		m_WindowSize = glm::vec2(500, 500);
		m_FramePadding = glm::vec2(2, 2);

		m_CurrentPath = s_AssetsPath;
		UpdatePaths();
	}

	void ContentBrowserWindow::Update()
	{
		if (m_UpdatePaths)
		{
			UpdatePaths();
		}
	}

	void ContentBrowserWindow::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(m_WindowSize.x, m_WindowSize.y), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("Content Browser", &m_Open))
		{
			ImGui::End();
			return;
		}

		UpdateWindowProperties();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_FramePadding.x, m_FramePadding.y));

		if (m_CurrentPath != s_AssetsPath)
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentPath = m_CurrentPath.parent_path();
				UpdatePaths();
			}
		}

		for (auto& [path, isDirectory] : m_PathsToDisplay)
		{
			auto relativePath = std::filesystem::relative(path);
			std::string filename = relativePath.filename().string();

			if (isDirectory)
			{
				if (ImGui::Button(filename.c_str()))
				{
					m_CurrentPath = path;
					m_UpdatePaths = true;
				}
			}
			else
			{
				if (relativePath.extension() == ".yaml")
				{
					if (ImGui::Button(filename.c_str()))
					{
						SceneManager::LoadScene(path.string());
					}
				}
				else
				{
					ImGui::Text("%s", filename.c_str());
				}
			}
		}

		if (m_CursorInContentRegion)
			HandleContextMenu();

		ImGui::PopStyleVar();
		ImGui::End();
	}

	void ContentBrowserWindow::UpdatePaths()
	{
		m_UpdatePaths = false;
		m_PathsToDisplay.clear();

		for (auto& iter : std::filesystem::directory_iterator(m_CurrentPath))
		{
			m_PathsToDisplay.push_back(std::pair(iter.path(), iter.is_directory()));
		}
	}

	void ContentBrowserWindow::UpdateWindowProperties()
	{
		// Get the position and content region
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 min = ImGui::GetWindowContentRegionMin();
		ImVec2 max = ImGui::GetWindowContentRegionMax();
		ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
		glm::vec2 windowPad = glm::vec2(windowPadding.x, windowPadding.y);

		// Calculate the window content region min + max
		m_WindowPos = glm::vec2(pos.x, pos.y);
		m_WindowMin = glm::vec2(min.x, min.y) + m_WindowPos;
		m_WindowMax = glm::vec2(max.x, max.y) + m_WindowPos;

		// Update the window size
		glm::vec2 windowSize = m_WindowMax - m_WindowMin;
		m_WindowResized = windowSize != m_WindowSize;
		m_WindowSize = windowSize;

		// Check if the cursor is in the content region
		glm::vec2 cursorPos = Input::GetScreenSpaceMousePosition();
		m_CursorInContentRegion = (cursorPos.x >= m_WindowMin.x && cursorPos.x <= m_WindowMax.x) &&
			(cursorPos.y >= m_WindowMin.y && cursorPos.y <= m_WindowMax.y);
	}

	void ContentBrowserWindow::HandleContextMenu()
	{
		if (!m_ContextMenuOpen && Input::GetMouseButtonDown(MouseButton::Right))
			ImGui::OpenPopup("Context Menu: Content Browser");

		// TODO: Check its inside content area
		if (m_ContextMenuOpen = ImGui::BeginPopup("Context Menu: Content Browser"))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Material"))
				{
					AssetManager::CreateMaterial("Assets/Materials/MyMaterial.mat");
				}
				else if (ImGui::MenuItem("Mesh"))
				{
					
				}
				else if (ImGui::MenuItem("Scene"))
				{

				}
				else if (ImGui::MenuItem("Shader"))
				{

				}
				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}

	void ContentBrowserWindow::Destroy()
	{
		m_PathsToDisplay.clear();
	}
}