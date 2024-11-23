#pragma once
#include "Ref.h"
#include "EditorEnums.h"
#include "DockableWindow.h"
#include "Inspector.h"

namespace Odyssey
{
	struct IEventListener;
	struct GUISelectionChangedEvent;
	struct SceneLoadedEvent;

	class InspectorWindow : public DockableWindow
	{
	public:
		InspectorWindow() = default;
		InspectorWindow(size_t windowID) : DockableWindow("InspectorWindow", 0, glm::vec2(0), glm::vec2(0), glm::vec2(0)) { }
		InspectorWindow(Ref<Inspector> inspector);

	public:
		virtual bool Draw() override;
		virtual void OnWindowClose() override;

	public:
		void OnGUISelectionChanged(GUISelectionChangedEvent* event);
		void OnSceneLoaded(SceneLoadedEvent* event);

		void ClearSelection();

	private:
		Ref<Inspector> m_Inspector;
		Ref<IEventListener> m_selectionChangedListener;
		Ref<IEventListener> m_SceneLoadedListener;
		bool open = true;
	};
}