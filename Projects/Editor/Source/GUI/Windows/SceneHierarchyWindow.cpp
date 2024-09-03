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

		if (m_Scene)
		{
			m_Interactions.clear();

			SceneGraph& sceneGraph = m_Scene->GetSceneGraph();
			const SceneGraph::Node* sceneRoot = sceneGraph.GetSceneRoot();

			// Draw the scene root's children
			// Note: Their children will be drawn recursively
			for (auto& node : sceneRoot->Children)
			{
				DrawSceneNode(node);
			}

			if (m_CursorInContentRegion && m_Interactions.empty())
				HandleContextMenu();
		}

		ProcessInteractions();

		if (m_Deferred)
		{
			m_Deferred();
			m_Deferred = nullptr;
		}
		End();
	}

	void SceneHierarchyWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		m_Scene = event->loadedScene;
	}

	void SceneHierarchyWindow::DrawSceneNode(const std::shared_ptr<SceneGraph::Node> node)
	{
		GameObject& entity = node->Entity;
		bool isLeaf = node->Children.size() == 0;

		// Skip entities, and their children, who are marked as don't show in hierarchy
		if (entity.HasComponent<EditorPropertiesComponent>())
		{
			EditorPropertiesComponent& properties = entity.GetComponent<EditorPropertiesComponent>();
			if (!properties.ShowInHierarchy)
				return;
		}

		// Draw the node
		if (DrawGameObject(entity, isLeaf))
		{
			// Draw the node's children, if they exist
			for (auto& childNode : node->Children)
			{
				DrawSceneNode(childNode);
			}

			// Pop the tree if we have children
			if (!isLeaf)
				ImGui::TreePop();
		}
	}

	bool SceneHierarchyWindow::DrawGameObject(GameObject& gameObject, bool leaf)
	{
		// Setup the flags
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (leaf)
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (bool selected = m_Selected.Equals(gameObject))
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		void* id = &gameObject;
		bool open = leaf;

		open = ImGui::TreeNodeEx(id, nodeFlags, gameObject.GetName().c_str());

		if (ImGui::IsItemHovered())
			m_Interactions.push_back({ InteractionType::Hovered, &gameObject });

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_Interactions.push_back({ InteractionType::Selection, &gameObject });
			m_Selected = gameObject;
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Button("Delete"))
			{
				m_Deferred = [this, &gameObject] { gameObject.Destroy(); };
			}
			if (ImGui::Button("Child"))
			{
				m_Deferred = [this, &gameObject] { gameObject.SetParent(m_Selected); };
			}

			ImGui::EndPopup();
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

		return open;
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
	void SceneHierarchyWindow::ProcessInteractions()
	{
		for (auto& interaction : m_Interactions)
		{
			switch (interaction.Type)
			{
				case InteractionType::Selection:
				{
					// Dispatch an event with the game object's guid
					GUISelection selection;
					selection.Type = GameObject::Type;
					selection.GUID = interaction.Target->GetGUID();
					EventSystem::Dispatch<GUISelectionChangedEvent>(selection);
					break;
				}
				case InteractionType::ContextMenu:
				{
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::Button("Delete"))
							interaction.Target->Destroy();
						if (ImGui::Button("Child"))
						{
							interaction.Target->SetParent(m_Selected);
						}

						ImGui::EndPopup();
					}
				}
				default:
					break;
			}
		}
	}
}