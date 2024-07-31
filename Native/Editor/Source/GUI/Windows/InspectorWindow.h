#pragma once
#include "EditorEnums.h"
#include "DockableWindow.h"

namespace Odyssey
{
	class Inspector;
	struct IEventListener;
	struct GUISelectionChangedEvent;

	class InspectorWindow : public DockableWindow
	{
	public:
		InspectorWindow() = default;
		InspectorWindow(std::shared_ptr<Inspector> inspector);

	public:
		virtual void Draw() override;

	public:
		void OnGUISelectionChanged(GUISelectionChangedEvent* event);
		void ClearSelection();

	private:
		std::shared_ptr<Inspector> m_Inspector;
		std::shared_ptr<IEventListener> m_EventListener;
		bool open = true;
	};
}