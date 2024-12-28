#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"
#include "Enum.h"

namespace Odyssey
{
	Camera::Camera(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{
	}

	void Camera::Awake()
	{
		// Calculate our matrices
		CalculateInverseView();
		CalculateProjection();
	}

	void Camera::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", Camera::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Tag", Enum::ToString(m_Tag));
		componentNode.WriteData("Field of View", m_FieldOfView);
		componentNode.WriteData("Near Clip", m_NearClip);
		componentNode.WriteData("Far Clip", m_FarClip);
		componentNode.WriteData("Main Camera", m_MainCamera);
	}

	void Camera::Deserialize(SerializationNode& node)
	{
		std::string tag;
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Tag", tag);
		node.ReadData("Field of View", m_FieldOfView);
		node.ReadData("Near Clip", m_NearClip);
		node.ReadData("Far Clip", m_FarClip);
		node.ReadData("Main Camera", m_MainCamera);

		if (!tag.empty())
			m_Tag = Enum::ToEnum<Tag>(tag);

		CalculateProjection();
	}

	glm::mat4 Camera::GetInverseView()
	{
		CalculateInverseView();
		return m_InverseView;
	}

	glm::mat4 Camera::GetView()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			transform->SetScale(1, 1, 1);
			m_View = transform->GetWorldMatrix();
		}

		return m_View;
	}

	void Camera::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}

	void Camera::SetFieldOfView(float fov)
	{
		m_FieldOfView = fov;
		CalculateProjection();
	}

	void Camera::SetNearClip(float nearClip)
	{
		m_NearClip = nearClip;
		CalculateProjection();
	}

	void Camera::SetFarClip(float farClip)
	{
		m_FarClip = farClip;
		CalculateProjection();
	}

	void Camera::SetViewportSize(float width, float height)
	{
		if (m_Width != width || m_Height != height)
		{
			m_Width = width;
			m_Height = height;
			CalculateProjection();
		}
	}

	void Camera::CalculateProjection()
	{
		m_Projection = glm::perspectiveFovLH(glm::radians(m_FieldOfView), m_Width, m_Height, m_NearClip, m_FarClip);
		m_InverseProjection = glm::inverse(m_Projection);
	}

	void Camera::CalculateInverseView()
	{
		m_View = GetView();
		m_InverseView = glm::inverse(m_View);
	}
}