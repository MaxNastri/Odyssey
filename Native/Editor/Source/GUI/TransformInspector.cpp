#include "TransformInspector.h"
#include <Transform.h>
#include <ComponentManager.h>

namespace Odyssey::Editor
{
	TransformInspector::TransformInspector(Entities::GameObject go)
	{
		using namespace Entities;

		gameObject = go;

		if (Transform* transform = ComponentManager::GetComponent<Transform>(go))
		{
			positionDrawer = Vector3Drawer("Position", transform->position);
			rotationDrawer = Vector3Drawer("Rotation", transform->eulerRotation);
			scaleDrawer = Vector3Drawer("Scale", transform->scale);
		}
	}

	void TransformInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Transform"))
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

	void TransformInspector::RegisterCallbacks()
	{
		std::function<void(Vector3)> positionModified = [this](Vector3 position)
			{
				OnPositionModified(position);
			};
		positionDrawer.SetCallback(positionModified);

		std::function<void(Vector3)> rotationModified = [this](Vector3 rotation)
			{
				OnPositionModified(rotation);
			};
		rotationDrawer.SetCallback(rotationModified);

		std::function<void(Vector3)> scaleModified = [this](Vector3 scale)
			{
				OnScaleModified(scale);
			};
		scaleDrawer.SetCallback(scaleModified);
	}

	void TransformInspector::OnPositionModified(Vector3 position)
	{
		using namespace Entities;

		if (Transform* transform = ComponentManager::GetComponent<Transform>(gameObject))
		{
			transform->position = position;
		}
	}

	void TransformInspector::OnRotationModified(Vector3 rotation)
	{
		using namespace Entities;

		if (Transform* transform = ComponentManager::GetComponent<Transform>(gameObject))
		{
			transform->eulerRotation = rotation;
		}
	}

	void TransformInspector::OnScaleModified(Vector3 scale)
	{
		using namespace Entities;

		if (Transform* transform = ComponentManager::GetComponent<Transform>(gameObject))
		{
			transform->scale = scale;
		}
	}
}