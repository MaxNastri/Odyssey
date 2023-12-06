#include "TransformInspector.h"
#include <Transform.h>
#include <ComponentManager.h>
#include <imgui.h>
#include <glm.h>

namespace Odyssey
{
	TransformInspector::TransformInspector(GameObject* go)
	{
		gameObject = go;

		if (Transform* transform = ComponentManager::GetComponent<Transform>(go->id))
		{
			// Callback for when the position is modified through the drawer
			std::function<void(glm::vec3)> positionModified = [go](glm::vec3 position)
				{
					if (Transform* transform = ComponentManager::GetComponent<Transform>(go->id))
					{
						transform->position = position;
					}
				};

			// Callback for when the rotation is modified through the drawer
			std::function<void(glm::vec3)> rotationModified = [go](glm::vec3 rotation)
				{
					if (Transform* transform = ComponentManager::GetComponent<Transform>(go->id))
					{
						transform->SetRotation(rotation);
					}
				};

			// Callback for when the scale is modified through the drawer
			std::function<void(glm::vec3)> scaleModified = [go](glm::vec3 scale)
				{
					if (Transform* transform = ComponentManager::GetComponent<Transform>(go->id))
					{
						transform->scale = scale;
					}
				};

			positionDrawer = Vector3Drawer("Position", transform->position, positionModified);
			rotationDrawer = Vector3Drawer("Rotation", transform->eulerRotation, rotationModified);
			scaleDrawer = Vector3Drawer("Scale", transform->scale, scaleModified);
		}
	}

	void TransformInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginTable("TransformInspector", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
			{
				ImGui::TableSetupColumn("##A", 0, 0.4f);
				positionDrawer.Draw();
				ImGui::TableNextRow();
				rotationDrawer.Draw();
				ImGui::TableNextRow();
				scaleDrawer.Draw();
				ImGui::EndTable();
			}
		}

		ImGui::Separator();
	}
}