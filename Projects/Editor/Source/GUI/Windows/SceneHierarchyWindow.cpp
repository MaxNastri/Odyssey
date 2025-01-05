#include "SceneHierarchyWindow.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "Input.h"
#include "EventSystem.h"
#include "EditorEvents.h"
#include "PropertiesComponent.h"
#include "EditorComponents.h"
#include "RawBuffer.h"
#include "GUIManager.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "Prefab.h"
#include "AssetManager.h"
#include "Project.h"
#include "FileDialogs.h"

namespace Odyssey
{
	SceneHierarchyWindow::SceneHierarchyWindow(size_t windowID)
		: DockableWindow("Scene Hierarchy", windowID,
			glm::vec2(0, 0), glm::vec2(400, 450), glm::vec2(2, 2))
	{
		m_Scene = SceneManager::GetActiveScene();
		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>
			([this](SceneLoadedEvent* event) { OnSceneLoaded(event); });
	}

	bool SceneHierarchyWindow::Draw()
	{
		bool modified = false;

		if (!Begin())
			return modified;

		if (m_Scene)
		{
			m_Interactions.clear();

			SceneGraph& sceneGraph = m_Scene->GetSceneGraph();
			Ref<SceneNode>& sceneRoot = sceneGraph.GetSceneRoot();

			// Draw the scene root's children
			// Note: Their children will be drawn recursively
			for (Ref<SceneNode> node : sceneRoot->Children)
			{
				if (node)
					DrawSceneNode(node);
			}

			if (m_CursorInContentRegion && m_Interactions.empty())
				HandleContextMenu();
		}

		ProcessInteractions();
		HandleDragAndDropWindow();

		if (m_Deferred)
		{
			m_Deferred();
			m_Deferred = nullptr;
		}

		End();
		return modified;
	}

	void SceneHierarchyWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void SceneHierarchyWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		m_Scene = event->loadedScene;
	}

	void SceneHierarchyWindow::DrawSceneNode(Ref<SceneNode>& node)
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
			// Reverse iteration since the node's can destroy themselves mid-draw
			for (int32_t i = (int32_t)node->Children.size() - 1; i >= 0; i--)
			{
				DrawSceneNode(node->Children[i]);
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
			m_Interactions.push_back({ InteractionType::Hovered, gameObject.GetGUID() });

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_Interactions.push_back({ InteractionType::Selection, gameObject.GetGUID() });
			m_Selected = gameObject;
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Button("Save as Prefab"))
			{
				const Path& path = FileDialogs::SaveFile("Prefab", ".prefab");
				Ref<Prefab> prefab = AssetManager::CreateAsset<Prefab>(path, gameObject);
			}
			if (ImGui::Button("Delete"))
			{
				gameObject.Destroy();
				ImGui::EndPopup();

				if (open && !leaf)
					ImGui::TreePop();

				return false;
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
			{
				// Register a drag and drop interaction
				Interaction<GUID>& interaction = m_Interactions.emplace_back();
				interaction.Type = InteractionType::DragAndDropTarget;
				interaction.Target = gameObject.GetGUID();
				uint64_t* data = (uint64_t*)payload->Data;
				GUID guid = GUID(*data);
				interaction.Data.Allocate(sizeof(GUID));
				interaction.Data.Write(&guid);
			}
			ImGui::EndDragDropTarget();
		}

		// Allow for this entity to be a potential drag/drop payload
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
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
				if (ImGui::MenuItem("Empty Object"))
				{
					GameObject gameObject = m_Scene->CreateGameObject();
				}
				if (ImGui::MenuItem("3D Object"))
				{
					GameObject gameObject = m_Scene->CreateGameObject();
					gameObject.AddComponent<Transform>();
					gameObject.AddComponent<MeshRenderer>();
				}
				if (ImGui::MenuItem("2D Object"))
				{
					GameObject gameObject = m_Scene->CreateGameObject();
					gameObject.AddComponent<Transform>();
					gameObject.AddComponent<SpriteRenderer>();
				}
				if (ImGui::MenuItem("Light"))
				{
					GameObject gameObject = m_Scene->CreateGameObject();
					gameObject.AddComponent<Transform>();
					gameObject.AddComponent<Light>();
				}
				if (ImGui::MenuItem("Particle Emitter"))
				{
					GameObject gameObject = m_Scene->CreateGameObject();
					gameObject.AddComponent<Transform>();
					gameObject.AddComponent<ParticleEmitter>();
				}
				if (ImGui::MenuItem("Camera"))
				{
					GameObject gameObject = m_Scene->CreateGameObject();
					gameObject.AddComponent<Transform>();
					gameObject.AddComponent<Camera>();
				}
				if (ImGui::MenuItem("Prefab"))
				{
					const Path& path = FileDialogs::OpenFile("Prefab", ".prefab");
					Ref<Prefab> prefab = AssetManager::LoadAsset<Prefab>(path);
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
	void SceneHierarchyWindow::HandleDragAndDropWindow()
	{
		ImRect rect;
		rect.Min = ImVec2(m_ContentRegionMin.x, m_ContentRegionMin.y);
		rect.Max = ImVec2(m_ContentRegionMax.x, m_ContentRegionMax.y);
		if (ImGui::BeginDragDropTargetCustom(rect, 123))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
			{
				GUID guid = *((GUID*)payload->Data);
				GameObject gameObject = m_Scene->GetGameObject(guid);
				if (gameObject.GetParent().IsValid())
					gameObject.RemoveParent();
			}
			ImGui::EndDragDropTarget();
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
					selection.GUID = interaction.Target;
					EventSystem::Dispatch<GUISelectionChangedEvent>(selection);
					break;
				}
				case InteractionType::DragAndDropTarget:
				{
					GUID guid = interaction.Data.Read<GUID>();
					GameObject gameObject = m_Scene->GetGameObject(guid);
					GameObject target = m_Scene->GetGameObject(interaction.Target);
					gameObject.SetParent(target);
					break;
				}
				default:
					break;
			}
		}
	}
}