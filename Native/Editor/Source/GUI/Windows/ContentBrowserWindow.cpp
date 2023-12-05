#include "ContentBrowserWindow.h"
#include "imgui.h"
#include "SceneManager.h"

namespace Odyssey
{
	static std::filesystem::path s_AssetsPath = "Assets";

	ContentBrowserWindow::ContentBrowserWindow()
	{
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
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Content Browser", &m_Open))
		{
			ImGui::End();
			return;
		}

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

	void ContentBrowserWindow::Destroy()
	{
		m_PathsToDisplay.clear();
	}
}