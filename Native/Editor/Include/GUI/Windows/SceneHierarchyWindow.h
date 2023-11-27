#pragma once
#include "GUIElement.h"

namespace Odyssey
{
	class Scene;

	class SceneHierarchyWindow : public GUIElement
	{
	public:
		SceneHierarchyWindow() = default;
		SceneHierarchyWindow(std::shared_ptr<Scene> scene);

	public:
		virtual void Draw() override;

	public:
		void OnGameObjectSelected(std::function<void(uint32_t)> callback) { m_OnGameObjectSelected.push_back(callback); }

	private:
		bool m_Open = true;
		bool m_NewObjectSelected = false;
		std::shared_ptr<Scene> m_Scene;
		std::vector<std::function<void(uint32_t)>> m_OnGameObjectSelected;
	};
}