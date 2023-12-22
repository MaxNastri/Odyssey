#include "TransformInspector.h"
#include "GameObject.h"
#include <Transform.h>
#include <ComponentManager.h>
#include <imgui.h>
#include <glm.h>

namespace Odyssey
{
	TransformInspector::TransformInspector(GameObject* gameObject)
	{
		m_GameObject = gameObject;

		if (Transform* transform = gameObject->GetComponent<Transform>())
		{
			positionDrawer = Vector3Drawer("Position", transform->m_Position, glm::vec3(0, 0, 0),
				[gameObject](glm::vec3 position) { OnPositionChanged(gameObject, position); });

			rotationDrawer = Vector3Drawer("Rotation", transform->m_EulerRotation, glm::vec3(0, 0, 0),
				[gameObject](glm::vec3 rotation) { OnRotationChanged(gameObject, rotation); });

			scaleDrawer = Vector3Drawer("Scale", transform->m_Scale, glm::vec3(1, 1, 1),
				[gameObject](glm::vec3 scale) { OnScaleChanged(gameObject, scale); });
		}
	}

	void TransformInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			positionDrawer.Draw();
			rotationDrawer.Draw();
			scaleDrawer.Draw();
		}

		ImGui::Separator();
	}

	void TransformInspector::OnPositionChanged(GameObject* gameObject, glm::vec3 position)
	{
		if (Transform* transform = gameObject->GetComponent<Transform>())
		{
			transform->m_Position = position;
		}
	}

	void TransformInspector::OnRotationChanged(GameObject* gameObject, glm::vec3 rotation)
	{
		if (Transform* transform = gameObject->GetComponent<Transform>())
		{
			transform->SetRotation(rotation);
		}
	}

	void TransformInspector::OnScaleChanged(GameObject* gameObject, glm::vec3 scale)
	{
		if (Transform* transform = gameObject->GetComponent<Transform>())
		{
			transform->m_Scale = scale;
		}
	}
}