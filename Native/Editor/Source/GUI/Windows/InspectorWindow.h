#pragma once
#include "EditorEnums.h"
#include "DockableWindow.h"

namespace Odyssey
{
	class Inspector;

	class InspectorWindow : public DockableWindow
	{
	public:
		InspectorWindow() = default;
		InspectorWindow(std::shared_ptr<Inspector> inspector);

	public:
		virtual void Draw() override;

	public:
		virtual void OnSelectionContextChanged(const GUISelection& context) override;
		void ClearSelection();

	private:
		std::shared_ptr<Inspector> m_Inspector;
		bool open = true;
	};
}