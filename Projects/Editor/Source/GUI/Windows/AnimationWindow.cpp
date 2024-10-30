#include "AnimationWindow.h"
#include "AnimationBlueprint.h"
#include "AssetManager.h"
#include "GUIManager.h"
#include "Input.h"
#include "FileDialogs.h"

namespace Odyssey
{
	AnimationWindow::AnimationWindow(size_t windowID)
		: DockableWindow("Animation Window", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		m_Blueprint = AssetManager::CreateAsset<AnimationBlueprint>("");
		m_WindowFlags = ImGuiWindowFlags_MenuBar;
		m_Builder = std::make_shared<BlueprintBuilder>(m_Blueprint.get());
		m_Builder->OverrideCreateNodeMenu(CreateNodeMenu::Menu_Name, CreateNodeMenu::ID);
		m_Builder->OverrideCreateLinkMenu(AddAnimationLinkMenu::Menu_Name, AddAnimationLinkMenu::ID);
	}

	void AnimationWindow::Destroy()
	{

	}

	void AnimationWindow::Update()
	{
		m_Blueprint->Update();
	}

	void AnimationWindow::Draw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, float2(0.0f, 0.0f));

		if (!Begin())
		{
			ImGui::PopStyleVar();
			return;
		}

		// Draw the menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Blueprint"))
				{
					m_Blueprint = AssetManager::CreateAsset<AnimationBlueprint>("");
				}
				else if (ImGui::MenuItem("Open Blueprint"))
				{
					const Path& path = FileDialogs::OpenFile("Animation Blueprint", ".rune");
					if (!path.empty())
						m_Blueprint = AssetManager::LoadAsset<AnimationBlueprint>(path);
				}
				else if (ImGui::MenuItem("Save Blueprint"))
				{
					if (!m_Blueprint->GetAssetPath().empty())
					{
						m_Blueprint->Save();
					}
					else
					{
						const Path& path = FileDialogs::SaveFile("Animation Blueprint", ".rune");
						if (!path.empty())
						{
							m_Blueprint->SetAssetPath(path);
							m_Blueprint->Save();
						}
					}
				}
				else if (ImGui::MenuItem("Save Blueprint To..."))
				{
					Path path = FileDialogs::SaveFile("Animation Blueprint", ".rune");
					if (!path.empty())
					{
						if (!path.has_extension())
							path = path / Path(".rune");

						m_Blueprint->SetAssetPath(path);
						m_Blueprint->Save();
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Host a dockspace for the node editor and it's panels
		m_DockspaceID = ImGui::GetID("AnimationWindowDS");
		if (!ImGui::DockBuilderGetNode(m_DockspaceID))
		{
			ImGui::DockBuilderRemoveNode(m_DockspaceID);
			ImGui::DockBuilderAddNode(m_DockspaceID);
			ImGui::DockBuilderSetNodeSize(m_DockspaceID, m_WindowSize);
			ImGui::DockBuilderFinish(m_DockspaceID);
		}
		ImGui::DockSpace(m_DockspaceID);
		End();

		// Pop the window padding
		ImGui::PopStyleVar();

		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		// Draw the node editor
		if (ImGui::Begin("Node Editor"))
		{
			m_Builder->SetEditor();

			m_UI.Draw(m_Blueprint.get(), m_Builder.get());

			// Begin building the UI
			m_Builder->Begin();

			// Draw the blueprint
			m_Builder->DrawBlueprint();

			// End building the UI
			m_Builder->End();
			ImGui::End();
		}
	}

	void AnimationWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}
}