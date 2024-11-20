#pragma once
#include "EditorEnums.h"
#include "DockableWindow.h"

namespace Odyssey
{
	class Inspector;
	struct IEventListener;
	struct GUISelectionChangedEvent;
	struct SceneLoadedEvent;

	class InspectorWindow : public DockableWindow
	{
	public:
		InspectorWindow() = default;
		InspectorWindow(size_t windowID) : DockableWindow("InspectorWindow", 0, glm::vec2(0), glm::vec2(0), glm::vec2(0)) { }
		InspectorWindow(std::shared_ptr<Inspector> inspector);

	public:
		virtual bool Draw() override;
		virtual void OnWindowClose() override;

	public:
		void OnGUISelectionChanged(GUISelectionChangedEvent* event);
		void OnSceneLoaded(SceneLoadedEvent* event);

		void ClearSelection();

	private:
		std::shared_ptr<Inspector> m_Inspector;
		std::shared_ptr<IEventListener> m_selectionChangedListener;
		std::shared_ptr<IEventListener> m_SceneLoadedListener;
		bool open = true;
	};
}