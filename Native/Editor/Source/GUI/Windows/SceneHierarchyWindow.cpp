#include "SceneHierarchyWindow.h"
#include "imgui.h"
#include "Scene.h"
#include "GameObject.h"

namespace Odyssey
{
	SceneHierarchyWindow::SceneHierarchyWindow(std::shared_ptr<Scene> scene)
	{
		m_Scene = scene;
	}

	void SceneHierarchyWindow::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Scene Hierarchy Window", &m_Open))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		// TODO (MAX): Draw
		uint32_t id = 0;

		static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selectionMask = (1 << 2);
		int node_clicked = -1;

		for (auto& gameObject : m_Scene->GetGameObjects())
		{
			bool hasChildren = false;
			const bool isSelected = (selectionMask & (1 << id)) != 0;
			ImGuiTreeNodeFlags nodeFlags = baseFlags;

			if (hasChildren)
			{
				// Draw as tree node
				bool open = ImGui::TreeNodeEx((void*)(intptr_t)id, nodeFlags, gameObject->name.c_str());

				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					node_clicked = id;

				if (open)
				{
					// Draw child

					// ImGui::TreePop();
				}
			}
			else
			{
				// Draw as tree leaf
				nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				ImGui::TreeNodeEx((void*)(intptr_t)id, nodeFlags, gameObject->name.c_str());

				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					node_clicked = id;

					for (auto& callback : m_OnGameObjectSelected)
					{
						callback(gameObject->id);
					}
				}
			}
		}

		if (node_clicked != -1)
		{
			if (ImGui::GetIO().KeyCtrl)
				selectionMask ^= (1 << node_clicked);
			else
				selectionMask = (1 << node_clicked);
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}
}