#pragma once
#include "GUIElement.h"

namespace Odyssey
{
	class Scene;

	class SceneHierarchyWindow : public GUIElement
	{
	public:
		SceneHierarchyWindow();

	public:
		virtual void Draw() override;

	public:
		void OnGameObjectSelected(std::function<void(uint32_t)> callback) { m_OnGameObjectSelected.push_back(callback); }
		void OnSceneChanged();

	private:
		bool m_Open = true;
		bool m_NewObjectSelected = false;
		Scene* m_Scene;
		std::vector<std::function<void(uint32_t)>> m_OnGameObjectSelected;
	};
}