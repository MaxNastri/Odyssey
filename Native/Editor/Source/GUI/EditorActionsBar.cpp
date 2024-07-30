#include "EditorActionsBar.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "EventSystem.h"
#include "EditorEvents.h"

namespace Odyssey
{
	EditorActionsBar::EditorActionsBar()
	{
		m_WindowFlags = 0;
		m_WindowFlags |= ImGuiWindowFlags_NoTitleBar;
		m_WindowFlags |= ImGuiWindowFlags_NoScrollbar;
		m_WindowFlags |= ImGuiWindowFlags_NoMove;
		m_WindowFlags |= ImGuiWindowFlags_NoResize;
		m_WindowFlags |= ImGuiWindowFlags_NoCollapse;
	}

	void EditorActionsBar::Draw()
	{
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = 
			ImGuiDockNodeFlags_NoTabBar | 
			ImGuiDockNodeFlags_NoDockingSplit | 
			ImGuiDockNodeFlags_NoResize;
		
		ImGui::SetNextWindowClass(&window_class);

		if (ImGui::Begin("Editor Actions Bar", &m_WindowOpen, m_WindowFlags))
		{
			// note that 0 tells Dear ImGui "just use the default"
			ImVec2 button_size = ImVec2{ 50, 30 };
			ImVec2 windowSize = ImGui::GetWindowSize();

			ImVec2 center
			{
				// we have two buttons, so twice the size - and we need to account for the spacing in the middle
				(windowSize.x - button_size.x * 3 - ImGui::GetStyle().ItemSpacing.x) / 2,
				(windowSize.y - button_size.y) / 2
			};

			// tell Dear ImGui to render the button at the new pos
			ImGui::SetCursorPos(center);

			if (ImGui::Button("Play", button_size))
				EventSystem::Dispatch<PlaymodeStateChangedEvent>(PlaymodeState::EnterPlaymode);

			ImGui::SameLine();
			
			ImGui::Button("||", button_size);
			
			ImGui::SameLine();

			if (ImGui::Button("Stop", button_size))
				EventSystem::Dispatch<PlaymodeStateChangedEvent>(PlaymodeState::ExitPlaymode);

			// Draw
			ImGui::End();
		}
	}
}