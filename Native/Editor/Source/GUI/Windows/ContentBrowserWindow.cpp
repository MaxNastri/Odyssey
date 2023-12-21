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

namespace Odyssey
{
	ContentBrowserWindow::ContentBrowserWindow()
		: DockableWindow("Content Browser",
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		m_CurrentPath = s_AssetsPath;
		UpdatePaths();
	}

	void ContentBrowserWindow::Destroy()
	{
		m_FoldersToDisplay.clear();
		m_FilesToDisplay.clear();
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
			auto relativePath = std::filesystem::relative(path);
			std::string filename = relativePath.filename().string();

			if (relativePath.extension() == ".yaml")
			{
				if (ImGui::Button(filename.c_str()))
				{
					const std::string& pathStr = path.string();

					SceneManager::LoadScene(pathStr);
				}
			}
			else
			{
				ImGui::PushID(filename.c_str());
				if (ImGui::Selectable(filename.c_str()))
				{
					GUISelection selection;
					selection.guid = AssetManager::PathToGUID(path);
					selection.Type = GUISelection::SelectionType::Material;
					GUIManager::OnSelectionContextChanged(selection);
				}
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					std::string guid = AssetManager::PathToGUID(path);
					ImGui::SetDragDropPayload("Asset", guid.c_str(), sizeof(guid));
					ImGui::EndDragDropSource();
				}
				ImGui::PopID();
			}
		}

		if (m_CursorInContentRegion)
			HandleContextMenu();

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
				m_FoldersToDisplay.push_back(iter.path());
			else if (iter.is_regular_file())
				m_FilesToDisplay.push_back(iter.path());
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
					AssetHandle<Shader> shader = AssetManager::CreateShader(std::filesystem::path("Assets/Shaders/Red_Frag.shader"));
					AssetHandle<Shader> shader2 = AssetManager::CreateShader(std::filesystem::path("Assets/Shaders/Red_Vert.shader"));
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
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}