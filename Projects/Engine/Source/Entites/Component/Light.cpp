#include "Light.h"
#include "Transform.h"

namespace Odyssey
{
	Light::Light(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	void Light::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", Light::Type);
		componentNode.WriteData("Light Type", (uint32_t)m_Type);
		componentNode.WriteData("Color", m_Color);
		componentNode.WriteData("Intensity", m_Intensity);
		componentNode.WriteData("Range", m_Range);
	}

	void Light::Deserialize(SerializationNode& node)
	{
		uint32_t lightType = 0;
		node.ReadData("Light Type", lightType);
		node.ReadData("Color", m_Color);
		node.ReadData("Intensity", m_Intensity);
		node.ReadData("Range", m_Range);

		m_Type = (LightType)lightType;
	}

	glm::vec3 Light::GetPosition()
	{
		glm::vec3 position = glm::vec3(0.0f);

		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			glm::mat4 world = transform->GetWorldMatrix();
			position = glm::vec3(world[3][0], world[3][1], world[3][2]);
		}

		return position;
	}

	glm::vec3 Light::GetDirection()
	{
		glm::vec3 rotation = glm::vec3(0.0f);

		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			glm::mat4 world = transform->GetWorldMatrix();
			rotation = glm::normalize(glm::column(world, 2));
		}

		return rotation;
	}
}