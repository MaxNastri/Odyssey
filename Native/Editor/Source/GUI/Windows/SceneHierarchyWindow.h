#pragma once
#include "DockableWindow.h"

namespace Odyssey
{
	class Scene;

	class SceneHierarchyWindow : public DockableWindow
	{
	public:
		SceneHierarchyWindow();

	public:
		virtual void Draw() override;

	public:
		void OnGameObjectSelected(std::function<void(int32_t)> callback) { m_OnGameObjectSelected.push_back(callback); }
		void OnSceneChanged();

	private:
		void HandleContextMenu();

	private: // Context menu
		bool m_ContextMenuOpen = false;

	private:
		bool m_Open = true;
		bool m_NewObjectSelected = false;
		Scene* m_Scene;
		std::vector<std::function<void(int32_t)>> m_OnGameObjectSelected;
	};
}