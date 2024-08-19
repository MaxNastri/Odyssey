#include "SceneHierarchyWindow.h"
#include "imgui.h"
#include "Scene.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "Input.h"
#include "EventSystem.h"
#include "EditorEvents.h"
#include "IDComponent.h"
#include "EditorComponents.h"

namespace Odyssey
{
	SceneHierarchyWindow::SceneHierarchyWindow()
		: DockableWindow("Scene Hierarchy",
			glm::vec2(0, 0), glm::vec2(400, 450), glm::vec2(2, 2))
	{
		m_Scene = SceneManager::GetActiveScene();
		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>
			([this](SceneLoadedEvent* event) { OnSceneLoaded(event); });
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
			for (auto entity : m_Scene->GetAllEntitiesWith<IDComponent>())
			{
				GameObject gameObject = GameObject(m_Scene, entity);

				// Don't display hidden game objects
				if (gameObject.HasComponent<EditorPropertiesComponent>() )
				{
					auto comp = gameObject.GetComponent<EditorPropertiesComponent>();
					if (!comp.ShowInHierarchy)
						continue;
				}

				bool hasChildren = false;
				const bool isSelected = (selectionMask & (1 << selectionID)) != 0;
				ImGuiTreeNodeFlags nodeFlags = baseFlags;

				if (hasChildren)
				{
					// Draw as tree node
					bool open = ImGui::TreeNodeEx((void*)(intptr_t)selectionID, nodeFlags, gameObject.GetName().c_str());

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
					ImGui::TreeNodeEx((void*)(intptr_t)selectionID, nodeFlags, gameObject.GetName().c_str());

					if (ImGui::IsItemDeactivated() && ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen())
					{
						nodeClicked = selectionID;

						GUISelection selection;
						selection.Type = GameObject::Type;
						selection.GUID = gameObject.GetGUID();
						EventSystem::Dispatch<GUISelectionChangedEvent>(selection);
					}
				}

				// Allow for this entity to be a potential draw/drop payload
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					uint64_t guid = gameObject.GetGUID();
					ImGui::SetDragDropPayload("Entity", (void*)&guid, sizeof(uint64_t));
					ImGui::EndDragDropSource();
				}
				++selectionID;
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
	
	void SceneHierarchyWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		m_Scene = event->loadedScene;
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
					m_Scene->CreateGameObject();
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}