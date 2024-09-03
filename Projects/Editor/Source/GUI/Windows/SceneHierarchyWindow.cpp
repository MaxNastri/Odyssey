#include "SceneHierarchyWindow.h"
#include "imgui.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "Input.h"
#include "EventSystem.h"
#include "EditorEvents.h"
#include "PropertiesComponent.h"
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
			return;

		uint32_t selectionID = 0;
		static int selectionMask = (1 << 2);
		int nodeClicked = -1;

		if (m_Scene)
		{
			m_Interactions.clear();

			for (auto entity : m_Scene->GetAllEntitiesWith<PropertiesComponent>())
			{
				GameObject gameObject = GameObject(m_Scene, entity);

				// Don't display hidden game objects
				if (gameObject.HasComponent<EditorPropertiesComponent>() )
				{
					EditorPropertiesComponent& properties = gameObject.GetComponent<EditorPropertiesComponent>();
					if (!properties.ShowInHierarchy)
						continue;
				}

				if (DrawGameObject(gameObject, selectionMask, selectionID))
					nodeClicked = selectionID;

				++selectionID;
			}

			if (m_CursorInContentRegion && m_Interactions.empty())
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

	bool SceneHierarchyWindow::DrawGameObject(GameObject& gameObject, int32_t& selectionMask, uint32_t& selectionID)
	{
		bool clicked = false;
		bool hasChildren = false;
		const bool isSelected = (selectionMask & (1 << selectionID)) != 0;
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (hasChildren)
		{
			// Draw as tree node
			bool open = ImGui::TreeNodeEx((void*)(intptr_t)selectionID, nodeFlags, gameObject.GetName().c_str());

			if (ImGui::IsItemHovered())
				m_Interactions.push_back({ InteractionType::Hovered, &gameObject });

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				clicked = true;

			if (open)
			{
				// Draw child

				// ImGui::TreePop();
			}
		}
		else
		{
			// Draw as tree leaf with no children
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx((void*)(intptr_t)selectionID, nodeFlags, gameObject.GetName().c_str());

			// Register a hovered interaction
			if (ImGui::IsItemHovered())
				m_Interactions.push_back({ InteractionType::Hovered, &gameObject });

			// Check if we should display a context menu
			if (ImGui::BeginPopupContextItem())
			{
				clicked = true;
				m_Interactions.push_back({ InteractionType::ContextMenu, &gameObject });

				if (ImGui::Button("Delete"))
					gameObject.Destroy();
				if (ImGui::Button("Child"))
				{
					auto view = m_Scene->GetAllEntitiesWith<PropertiesComponent>();
					auto entity = view.begin()[selectionID];
					GameObject parent = GameObject(m_Scene, entity);
					gameObject.SetParent(parent);
				}

				ImGui::EndPopup();
			}

			// Check if this item has been selected
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				clicked = true;
				m_Interactions.push_back({ InteractionType::Selection, &gameObject });

				// Dispatch an event with the game object's guid
				GUISelection selection;
				selection.Type = GameObject::Type;
				selection.GUID = gameObject.GetGUID();
				EventSystem::Dispatch<GUISelectionChangedEvent>(selection);
			}
		}

		// Allow for this entity to be a potential drag/drop payload
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			// Register a drag and drop interaction
			m_Interactions.push_back({ InteractionType::DragAndDrop, &gameObject });

			uint64_t guid = gameObject.GetGUID();
			ImGui::SetDragDropPayload("Entity", (void*)&guid, sizeof(uint64_t));
			ImGui::EndDragDropSource();
		}

		return clicked;
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
					GameObject gameObject = m_Scene->CreateGameObject();
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}