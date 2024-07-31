#include "ContentBrowserWindow.h"
#include "Input.h"
#include "imgui.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "GeometryUtil.h"
#include "Mesh.h"
#include "GUIManager.h"
#include "ModelLoader.h"
#include "Scene.h"
#include "Texture2D.h"
#include "Material.h"
#include "ProjectManager.h"

namespace Odyssey
{
	ContentBrowserWindow::ContentBrowserWindow()
		: DockableWindow("Content Browser",
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		m_AssetsPath = ProjectManager::GetAssetsDirectory();
		m_CurrentPath = m_AssetsPath;
		
		TrackingOptions options;
		options.Direrctory = m_AssetsPath;
		options.Extensions = { ".asset", ".glsl", ".meta" };
		options.Recursive = true;
		options.IncludeDirectoryChanges = true;
		options.Callback = [this](const std::filesystem::path& filePath, FileActionType fileAction)
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
		for (auto& path : m_FoldersToDisplay)
		{
			auto relativePath = std::filesystem::relative(path);
			std::string filename = relativePath.filename().string();

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
					AssetManager::CreateMaterial(std::filesystem::path("Assets/Materials/MyMaterial.mat"));
				}
				if (ImGui::BeginMenu("Mesh"))
				{
					if (ImGui::MenuItem("Cube"))
					{
						std::vector<VulkanVertex> vertices;
						std::vector<uint32_t> indices;
						GeometryUtil::ComputeBox(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), vertices, indices);
						AssetHandle<Mesh> mesh = AssetManager::CreateMesh(std::filesystem::path("Assets/Meshes/Cube.mesh"));
						mesh.Get()->SetVertices(vertices);
						mesh.Get()->SetIndices(indices);
						mesh.Get()->Save();
					}
					if (ImGui::MenuItem("Sphere"))
					{
						std::vector<VulkanVertex> vertices;
						std::vector<uint32_t> indices;
						GeometryUtil::ComputeSphere(1.0f, 50, vertices, indices);
						AssetHandle<Mesh> mesh = AssetManager::CreateMesh(std::filesystem::path("Assets/Meshes/Sphere.mesh"));
						mesh.Get()->SetVertices(vertices);
						mesh.Get()->SetIndices(indices);
						mesh.Get()->Save();
					}
					ImGui::EndMenu();
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
				if (ImGui::MenuItem("Spiderman"))
				{
					const std::filesystem::path path("Assets/Models/Car_Combined.fbx");
					ModelLoader loader;
					ModelAsset asset;
					if (loader.LoadModel(path, asset))
					{
						GameObject* go = SceneManager::GetActiveScene()->GetGameObject(0);
						if (MeshRenderer* mr = go->GetComponent<MeshRenderer>())
						{
							mr->SetMesh(asset.Mesh);
						}
						//asset.Mesh.Get()->SaveTo("Assets/Meshes/Car.mesh");
					}
				}
				if (ImGui::MenuItem("Texture"))
				{
					const std::filesystem::path path("Assets/Textures/texture.jpg");
					AssetHandle<Texture2D> texture = AssetManager::LoadTexture2D(path);

					if (texture.IsValid())
					{
						GameObject* go = SceneManager::GetActiveScene()->GetGameObject(0);
						if (Material* material = go->GetComponent<MeshRenderer>()->GetMaterial().Get())
						{
							material->SetTexture(texture);
							material->Save();
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
	
	void ContentBrowserWindow::DrawSceneAsset(const std::filesystem::path& assetPath)
	{
		std::string filename = assetPath.filename().string();

		if (ImGui::Button(filename.c_str()))
		{
			SceneManager::LoadScene(assetPath.string());
		}
	}

	void ContentBrowserWindow::DrawSourceAsset(const std::filesystem::path& sourcePath)
	{
		std::string filename = sourcePath.filename().string();
		ImGui::PushID(filename.c_str());

		if (ImGui::Selectable(filename.c_str()))
		{
			// TODO: Use the asset manager to parse source file types
			GUISelection selection;
			selection.GUID = AssetManager::PathToGUID(sourcePath);
			selection.Type = AssetManager::GUIDToAssetType(selection.GUID);
			selection.FilePath = sourcePath;
			GUIManager::OnSelectionContextChanged(selection);
		}
		ImGui::PopID();
	}
	void ContentBrowserWindow::DrawAsset(const std::filesystem::path& assetPath)
	{
		std::string filename = assetPath.filename().string();

		ImGui::PushID(filename.c_str());

		if (ImGui::Selectable(filename.c_str()))
		{
			GUISelection selection;
			selection.GUID = AssetManager::PathToGUID(assetPath);
			selection.Type = AssetManager::GUIDToAssetType(selection.GUID);
			GUIManager::OnSelectionContextChanged(selection);
		}

		// Allow for this asset to be a potential draw/drop payload
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			std::string guid = AssetManager::PathToGUID(assetPath);
			ImGui::SetDragDropPayload("Asset", guid.c_str(), sizeof(guid));
			ImGui::EndDragDropSource();
		}

		ImGui::PopID();
	}

	void ContentBrowserWindow::OnFileAction(const std::filesystem::path& filePath, FileActionType fileAction)
	{
		UpdatePaths();
	}
}