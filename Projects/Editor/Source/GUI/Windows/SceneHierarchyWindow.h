#pragma once
#include "DockableWindow.h"
#include "EventSystem.h"
#include "Interaction.hpp"
#include "SceneGraph.h"

namespace Odyssey
{
	class Scene;
	struct SceneLoadedEvent;

	class SceneHierarchyWindow : public DockableWindow
	{
	public:
		SceneHierarchyWindow();

	public:
		virtual void Draw() override;

	public:
		void OnSceneLoaded(SceneLoadedEvent* event);

	private:
		void DrawSceneNode(const std::shared_ptr<SceneGraph::Node> node);
		bool DrawGameObject(GameObject& gameObject, bool leaf);
		void HandleContextMenu();
		void HandleDragAndDropWindow();
		void ProcessInteractions();

	private:
		Scene* m_Scene;
		std::vector<Interaction<GameObject>> m_Interactions;
		bool m_ContextMenuOpen = false;
		std::shared_ptr<IEventListener> m_SceneLoadedListener;
		GameObject m_Selected;
		std::function<void(void)> m_Deferred;
	};
}