#include "Camera.h"
#include "ComponentManager.h"
#include "Transform.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, Camera);

	void Camera::Awake()
	{
		// Cache the transform
		m_Transform = ComponentManager::GetComponent<Transform>(gameObject->id);

		m_Width = 1000;
		m_Height = 1000;
		// Calculate our matrices
		CalculateInverseView();
		CalculateProjection();
	}

	void Camera::Serialize(ryml::NodeRef& node)
	{
		ryml::NodeRef componentNode = node.append_child();
		componentNode |= ryml::MAP;

		componentNode["Name"] << Camera::Type;
		componentNode["Field of View"] << m_FieldOfView;
		componentNode["Near Clip"] << m_NearClip;
		componentNode["Far Clip"] << m_FarClip;
		componentNode["Main Camera"] << m_MainCamera;
	}

	void Camera::Deserialize(ryml::ConstNodeRef node)
	{
		node["Field of View"] >> m_FieldOfView;
		node["Near Clip"] >> m_NearClip;
		node["Far Clip"] >> m_FarClip;
		node["Main Camera"] >> m_MainCamera;

		m_Width = 1000;
		m_Height = 1000;
		CalculateProjection();
	}

	glm::mat4 Camera::GetInverseView()
	{
		CalculateInverseView();
		return m_InverseView;
	}

	void Camera::SetFieldOfView(float fov)
	{
		m_FieldOfView = fov;
	}

	void Camera::SetNearClip(float nearClip)
	{
		m_NearClip = nearClip;
	}

	void Camera::SetFarClip(float farClip)
	{
		m_FarClip = farClip;
	}

	void Camera::CalculateProjection()
	{
		m_Projection = glm::perspectiveFovLH(m_FieldOfView, m_Width, m_Height, m_NearClip, m_FarClip);
		m_Projection[1][1] = -m_Projection[1][1];
	}

	void Camera::CalculateInverseView()
	{
		if (m_Transform == nullptr)
			m_Transform = ComponentManager::GetComponent<Transform>(gameObject->id);

		m_InverseView = glm::inverse(m_Transform->GetWorldMatrix());
	}
}