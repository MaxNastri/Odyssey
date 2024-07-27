#include "SceneHierarchyWindow.h"
#include "imgui.h"
#include "Scene.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "Input.h"

namespace Odyssey
{
	SceneHierarchyWindow::SceneHierarchyWindow()
		: DockableWindow("Scene Hierarchy",
			glm::vec2(0, 0), glm::vec2(400, 450), glm::vec2(2, 2))
	{
		m_Scene = SceneManager::GetActiveScene();
	}

	void SceneHierarchyWindow::Draw()
	{
		if (!Begin())
		{
			return;
		}

		uint32_t selectionID = 0;

		static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selectionMask = (1 << 2);
		int nodeClicked = -1;

		if (m_Scene)
		{
			for (auto gameObject : m_Scene->GetGameObjects())
			{
				bool hasChildren = false;
				const bool isSelected = (selectionMask & (1 << selectionID)) != 0;
				ImGuiTreeNodeFlags nodeFlags = baseFlags;

				if (hasChildren)
				{
					// Draw as tree node
					bool open = ImGui::TreeNodeEx((void*)(intptr_t)selectionID, nodeFlags, gameObject->name.c_str());

					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
						nodeClicked = selectionID;

					if (open)
					{
						// Draw child

						// ImGui::TreePop();
					}
				}
				else
				{
					// Draw as tree leaf
					nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					ImGui::TreeNodeEx((void*)(intptr_t)selectionID, nodeFlags, gameObject->name.c_str());

					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					{
						nodeClicked = selectionID;

						GUISelection selection;
						selection.Type = GameObject::Type;
						selection.ID = gameObject->id;

						GUIManager::OnSelectionContextChanged(selection);
					}
				}
			}

			if (m_CursorInContentRegion)
				HandleContextMenu();
		}
		
		if (nodeClicked != -1)
		{
			if (ImGui::GetIO().KeyCtrl)
				selectionMask ^= (1 << nodeClicked);
			else
				selectionMask = (1 << nodeClicked);
		}

		End();
	}
	
	void SceneHierarchyWindow::OnSceneChanged()
	{
		m_Scene = SceneManager::GetActiveScene();
	}

	void SceneHierarchyWindow::HandleContextMenu()
	{
		if (!m_ContextMenuOpen && Input::GetMouseButtonDown(MouseButton::Right))
			ImGui::OpenPopup("SceneHierarchyWindow");

		if (m_ContextMenuOpen = ImGui::BeginPopup("SceneHierarchyWindow"))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("GameObject"))
				{
					GameObject* created = m_Scene->CreateGameObject();
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}