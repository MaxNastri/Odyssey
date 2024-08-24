#pragma once
#include "DockableWindow.h"
#include "EventSystem.h"
#include "Interaction.hpp"

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
		void OnGameObjectSelected(std::function<void(int32_t)> callback) { m_OnGameObjectSelected.push_back(callback); }
		void OnSceneLoaded(SceneLoadedEvent* event);

	private:
		bool DrawGameObject(GameObject& gameObject, int32_t& selectionMask, uint32_t& selectionID);
		void HandleContextMenu();

	private:
		std::vector<Interaction<GameObject>> m_Interactions;
		bool m_ContextMenuOpen = false;

	private:
		Scene* m_Scene;
		std::shared_ptr<IEventListener> m_SceneLoadedListener;
		bool m_NewObjectSelected = false;
		std::vector<std::function<void(int32_t)>> m_OnGameObjectSelected;
	};
}