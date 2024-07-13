#include "DockableWindow.h"
#include "imgui.h"
#include "Input.h"

namespace Odyssey
{
	DockableWindow::DockableWindow(const std::string& windowName, glm::vec2 position, glm::vec2 size, glm::vec2 framePadding)
	{
		m_WindowName = windowName;
		m_WindowPos = position;
		m_WindowSize = size;
		m_FramePadding = framePadding;
	}

	bool DockableWindow::Begin()
	{
		ImGui::SetNextWindowSize(ImVec2(m_WindowSize.x, m_WindowSize.y), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin(m_WindowName.c_str(), &m_Open))
		{
			ImGui::End();
			return false;
		}

		// Update the window properties
		UpdateWindowProperties();

		// Push the frame padding
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_FramePadding.x, m_FramePadding.y));

		return true;
	}

	void DockableWindow::End()
	{
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void DockableWindow::UpdateWindowProperties()
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
		ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
		ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;

		// Position and content region
		m_WindowPos = glm::vec2(windowPos.x, windowPos.y);
		m_WindowPadding = glm::vec2(windowPadding.x, windowPadding.y);
		m_ContentRegionMin = glm::vec2(contentMin.x, contentMin.y) + m_WindowPos - m_FramePadding - m_WindowPadding;
		m_ContentRegionMax = glm::vec2(contentMax.x, contentMax.y) + m_WindowPos + m_FramePadding + m_WindowPadding;

		// Window sizing
		glm::vec2 windowMin = glm::vec2(contentMin.x, contentMin.y) + m_WindowPos;
		glm::vec2 windowMax = glm::vec2(contentMax.x, contentMax.y) + m_WindowPos;
		glm::vec2 windowSize = windowMax - windowMin;

		m_WindowResized = windowSize != m_WindowSize;
		m_WindowSize = windowSize;

		if (m_DebugEnabled)
		{
		}

		// Cursor update
		glm::vec2 cursorPos = Input::GetScreenSpaceMousePosition();
		m_CursorInContentRegion = (cursorPos.x >= m_ContentRegionMin.x && cursorPos.x <= m_ContentRegionMax.x) &&
			(cursorPos.y >= m_ContentRegionMin.y && cursorPos.y <= m_ContentRegionMax.y);

		if (m_DebugEnabled)
		{
			auto drawList = ImGui::GetForegroundDrawList();

			ImColor red = IM_COL32(255.0f, 0.0f, 0.0f, 255.0f);
			ImColor green = IM_COL32(0.0f, 255.0f, 0.0f, 255.0f);
			ImColor yellow = IM_COL32(255.0f, 255.0f, 0.0f, 255.0f);

			ImVec2 cursor = ImVec2(cursorPos.x, cursorPos.y);
			ImVec2 contentMin = ImVec2(m_ContentRegionMin.x, m_ContentRegionMin.y);
			ImVec2 contentMax = ImVec2(m_ContentRegionMax.x, m_ContentRegionMax.y);

			// Draw an outline around the content region in red
			drawList->AddRect(contentMin, contentMax, yellow);

			// Draw a circle around the cursor pos colored to whether itsi n the content region or not
			drawList->AddCircle(cursor, 10.0f, m_CursorInContentRegion ? green : red);
		}

		// Fire the resize event if we have resized
		if (m_WindowResized)
		{
			OnWindowResize();
		}
	}
}