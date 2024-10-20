#include "AnimationWindow.h"
#include "GUIManager.h"
#include "Input.h"

namespace Odyssey
{
	AnimationWindow::AnimationWindow(size_t windowID)
		: DockableWindow("Animation Window", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
	}

	void AnimationWindow::Destroy()
	{

	}

	void AnimationWindow::Update()
	{
		bp.Update();
	}

	void AnimationWindow::Draw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, float2(0.0f, 0.0f));

		bool active = false;

		if (active = Begin())
		{
			ImGuiID dockspace_id = ImGui::GetID("AnimationWindowDS");

			if (!ImGui::DockBuilderGetNode(dockspace_id))
			{
				ImGui::DockBuilderRemoveNode(dockspace_id);
				ImGui::DockBuilderAddNode(dockspace_id);
				ImGui::DockBuilderSetNodeSize(dockspace_id, m_WindowSize);
				ImGui::DockBuilderFinish(dockspace_id);
			}

			ImGui::DockSpace(dockspace_id);
			End();
		}

		ImGui::PopStyleVar();

		if (active)
		{
			ImGuiWindowClass window_class;
			window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
			ImGui::SetNextWindowClass(&window_class);

			if (ImGui::Begin("Node Editor"))
			{
				bp.Draw();
				ImGui::End();
			}
		}
	}

	void AnimationWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}
}