#pragma once
#include "GUIElement.h"
#include "glm.h"
#include "EditorEnums.h"

namespace Odyssey
{
	class DockableWindow : public GUIElement
	{
	public:
		DockableWindow() = default;
		DockableWindow(const std::string& windowName, glm::vec2 position, glm::vec2 size, glm::vec2 framePadding);

	public:
		virtual void Update() { }
		virtual void Draw() { };
		virtual void OnWindowResize() { }
		virtual void OnSelectionContextChanged(const GUISelection& context) { }

	protected:
		bool Begin();
		void End();
		void UpdateWindowProperties();

	protected:
		bool m_Open = true;
		bool m_DebugEnabled = false;
		std::string m_WindowName;
		glm::vec2 m_WindowPos;
		glm::vec2 m_WindowSize;
		glm::vec2 m_ContentRegionMin;
		glm::vec2 m_ContentRegionMax;
		glm::vec2 m_WindowPadding;
		glm::vec2 m_FramePadding;
		bool m_CursorInContentRegion = false;
		bool m_WindowResized = false;
	};
}