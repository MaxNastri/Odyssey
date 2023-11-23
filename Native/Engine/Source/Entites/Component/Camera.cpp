#include "Camera.h"
#include "ComponentManager.h"
#include "Transform.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, Camera);

	void Camera::Awake()
	{
		// Cache the transform
		m_Transform = ComponentManager::GetComponent<Transform>(gameObject);

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

	}

	void Camera::Deserialize(ryml::ConstNodeRef node)
	{
		node["Field of View"] >> m_FieldOfView;
		node["Near Clip"] >> m_NearClip;
		node["Far Clip"] >> m_FarClip;
	}

	glm::mat4 Camera::GetInverseView()
	{
		CalculateInverseView();
		return m_InverseView;
	}

	void Camera::CalculateProjection()
	{
		m_Projection = glm::perspectiveFovLH(m_FieldOfView, m_Width, m_Height, m_NearClip, m_FarClip);
	}

	void Camera::CalculateInverseView()
	{
		m_InverseView = glm::inverse(m_Transform->GetWorldMatrix());
	}
}