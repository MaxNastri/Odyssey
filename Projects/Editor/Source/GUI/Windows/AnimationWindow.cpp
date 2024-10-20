#include "AnimationWindow.h"
#include "GUIManager.h"
#include "Input.h"

namespace Odyssey
{
	AnimationWindow::AnimationWindow(size_t windowID)
		: DockableWindow("Animation Window", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		m_Builder = std::make_shared<BlueprintBuilder>(&bp);
		m_Builder->OverrideCreateNodeMenu(CreateNodeMenu::Name, CreateNodeMenu::ID);
		m_Builder->OverrideCreateLinkMenu(AddAnimationLinkMenu::Name, AddAnimationLinkMenu::ID);
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
				m_Builder->SetEditor();

				m_UI.Draw(&bp, m_Builder.get());

				// Begin building the UI
				m_Builder->Begin();

				// Draw the blueprint
				m_Builder->DrawBlueprint();

				// End building the UI
				m_Builder->End();
				ImGui::End();
			}
		}
	}

	void AnimationWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}
}