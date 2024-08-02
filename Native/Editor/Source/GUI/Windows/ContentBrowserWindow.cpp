#include "ContentBrowserWindow.h"
#include "imgui.h"
#include "Input.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "Project.h"
#include "EventSystem.h"
#include "EditorEvents.h"

namespace Odyssey
{
	ContentBrowserWindow::ContentBrowserWindow()
		: DockableWindow("Content Browser",
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		m_AssetsPath = Project::GetActiveAssetsDirectory();
		m_CurrentPath = m_AssetsPath;
		
		TrackingOptions options;
		options.Direrctory = m_AssetsPath;
		options.Extensions = { ".asset", ".glsl", ".meta" };
		options.Recursive = true;
		options.IncludeDirectoryChanges = true;
		options.Callback = [this](const Path& filePath, FileActionType fileAction)
			{ OnFileAction(filePath, fileAction); };
		m_FileTracker = std::make_unique<FileTracker>(options);

		UpdatePaths();
	}

	void ContentBrowserWindow::Destroy()
	{
		m_FoldersToDisplay.clear();
		m_FilesToDisplay.clear();
	}

	void ContentBrowserWindow::Draw()
	{
		if (!Begin())
			return;

		if (m_CurrentPath != m_AssetsPath)
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentPath = m_CurrentPath.parent_path();
				UpdatePaths();
			}
		}

		// Draw folders first
		for (auto path : m_FoldersToDisplay)
		{
			std::string filename = path.filename().string();

			if (ImGui::Button(filename.c_str()))
			{
				m_CurrentPath = path;
				m_UpdatePaths = true;
			}
		}

		// Now files
		for (auto& path : m_FilesToDisplay)
		{
			if (path.extension() == ".scene")
			{
				// Scene path
				DrawSceneAsset(path);
			}
			else if (path.extension() == ".glsl") // Source asset lane
			{
				DrawSourceAsset(path);
			}
			else // Asset lane
			{
				DrawAsset(path);
			}
		}

		if (m_CursorInContentRegion)
			HandleContextMenu();

		if (m_UpdatePaths)
			UpdatePaths();
		End();
	}

	void ContentBrowserWindow::UpdatePaths()
	{
		m_UpdatePaths = false;
		m_FilesToDisplay.clear();
		m_FoldersToDisplay.clear();

		for (auto& iter : std::filesystem::directory_iterator(m_CurrentPath))
		{
			if (iter.is_directory())
			{
				m_FoldersToDisplay.push_back(iter.path());
			}
			else if (iter.is_regular_file())
			{
				if (iter.path().extension() != ".meta")
					m_FilesToDisplay.push_back(iter.path());
			}
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
					AssetManager::CreateMaterial(Path("Assets/Materials/MyMaterial.mat"));
				}
				if (ImGui::MenuItem("Scene"))
				{

				}
				if (ImGui::MenuItem("Shader"))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Import"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
	
	void ContentBrowserWindow::DrawSceneAsset(const Path& assetPath)
	{
		std::string filename = assetPath.filename().string();

		if (ImGui::Button(filename.c_str()))
		{
			SceneManager::LoadScene(assetPath.string());
		}
	}

	void ContentBrowserWindow::DrawSourceAsset(const Path& sourcePath)
	{
		std::string filename = sourcePath.filename().string();
		ImGui::PushID(filename.c_str());

		if (ImGui::Selectable(filename.c_str()))
		{
			GUISelection selection;
			selection.GUID = AssetManager::PathToGUID(sourcePath);
			selection.Type = AssetManager::GUIDToAssetType(selection.GUID);
			selection.FilePath = sourcePath;
			EventSystem::Dispatch<GUISelectionChangedEvent>(selection);
		}
		ImGui::PopID();
	}
	void ContentBrowserWindow::DrawAsset(const Path& assetPath)
	{
		std::string filename = assetPath.filename().string();

		ImGui::PushID(filename.c_str());

		if (ImGui::Selectable(filename.c_str()))
		{
			GUISelection selection;
			selection.GUID = AssetManager::PathToGUID(assetPath);
			selection.Type = AssetManager::GUIDToAssetType(selection.GUID);
			EventSystem::Dispatch<GUISelectionChangedEvent>(selection);
		}

		// Allow for this asset to be a potential draw/drop payload
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			GUID guid = AssetManager::PathToGUID(assetPath);
			ImGui::SetDragDropPayload("Asset", guid.String().c_str(), sizeof(guid));
			ImGui::EndDragDropSource();
		}

		ImGui::PopID();
	}

	void ContentBrowserWindow::OnFileAction(const Path& filePath, FileActionType fileAction)
	{
		UpdatePaths();
	}
}