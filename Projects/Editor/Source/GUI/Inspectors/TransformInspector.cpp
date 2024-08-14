#include "TransformInspector.h"
#include "GameObject.h"
#include "Transform.h"
#include "imgui.h"

namespace Odyssey
{
	TransformInspector::TransformInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			positionDrawer = Vector3Drawer("Position", transform->m_Position, glm::vec3(0, 0, 0),
				[this](glm::vec3 position) { OnPositionChanged(position); });

			rotationDrawer = Vector3Drawer("Rotation", transform->m_EulerRotation, glm::vec3(0, 0, 0),
				[this](glm::vec3 rotation) { OnRotationChanged(rotation); });

			scaleDrawer = Vector3Drawer("Scale", transform->m_Scale, glm::vec3(1, 1, 1),
				[this](glm::vec3 scale) { OnScaleChanged(scale); });
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
		ImGui::Spacing();
	}

	void TransformInspector::OnPositionChanged(glm::vec3 position)
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			transform->m_Position = position;
		}
	}

	void TransformInspector::OnRotationChanged(glm::vec3 rotation)
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			transform->SetRotation(rotation);
		}
	}

	void TransformInspector::OnScaleChanged(glm::vec3 scale)
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			transform->m_Scale = scale;
		}
	}
}