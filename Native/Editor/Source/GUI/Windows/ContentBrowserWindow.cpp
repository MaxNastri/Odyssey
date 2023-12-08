#include "ContentBrowserWindow.h"
#include "Input.h"
#include "imgui.h"
#include "SceneManager.h"
#include "AssetManager.h"

namespace Odyssey
{
	static std::filesystem::path s_AssetsPath = "Assets";

	ContentBrowserWindow::ContentBrowserWindow()
		: DockableWindow("Content Browser", 
			glm::vec2(0,0), glm::vec2(500,500), glm::vec2(2,2))
	{
		m_CurrentPath = s_AssetsPath;
		UpdatePaths();
	}

	void ContentBrowserWindow::Destroy()
	{
		m_PathsToDisplay.clear();
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
		if (!Begin())
			return;

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

		End();
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
}