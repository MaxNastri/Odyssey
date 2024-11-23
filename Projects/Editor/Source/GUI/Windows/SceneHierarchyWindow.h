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
		SceneHierarchyWindow(size_t windowID);

	public:
		virtual bool Draw() override;
		virtual void OnWindowClose() override;

	public:
		void OnSceneLoaded(SceneLoadedEvent* event);

	private:
		void DrawSceneNode(Ref<SceneNode>& node);
		bool DrawGameObject(GameObject& gameObject, bool leaf);
		void HandleContextMenu();
		void HandleDragAndDropWindow();
		void ProcessInteractions();

	private:
		Scene* m_Scene;
		std::vector<Interaction<GameObject>> m_Interactions;
		bool m_ContextMenuOpen = false;
		Ref<IEventListener> m_SceneLoadedListener;
		GameObject m_Selected;
		std::function<void(void)> m_Deferred;
	};
}