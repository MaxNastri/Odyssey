#include "Light.h"
#include "Transform.h"
#include "Enum.h"

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
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Light Type", Enum::ToString<LightType>(m_Type));
		componentNode.WriteData("Color", m_Color);
		componentNode.WriteData("Intensity", m_Intensity);
		componentNode.WriteData("Range", m_Range);
	}

	void Light::Deserialize(SerializationNode& node)
	{
		std::string lightType;
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Light Type", lightType);
		node.ReadData("Color", m_Color);
		node.ReadData("Intensity", m_Intensity);
		node.ReadData("Range", m_Range);

		m_Type = Enum::ToEnum<LightType>(lightType);
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

	mat4 Light::CalculateViewProj(float3 sceneCenter, float sceneRadius, float4 lightPos, float4 lightDir)
	{
		float distance = -2.0f * sceneRadius;
		float3 position = lightDir * distance;

		// Get the light space view matrix
		mat4 view = glm::inverse(glm::lookAtLH(position, sceneCenter, float3(0.0f, 1.0f, 0.0f)));

		// Calculate the scene center in light space
		float4 lightSpaceCenter = view * float4(sceneCenter, 1.0f);

		// Get the bounds of the orthographic projection
		float left = lightSpaceCenter.x - sceneRadius;
		float right = lightSpaceCenter.x + sceneRadius;

		float bottom = lightSpaceCenter.y - sceneRadius;
		float top = lightSpaceCenter.y + sceneRadius;

		float zNear = lightSpaceCenter.z - sceneRadius;
		float zFar = lightSpaceCenter.z + sceneRadius;

		// Calculate the orthographic projection
		mat4 proj = glm::orthoLH(left, right, bottom, top, .01f, 100.0f);
		proj[1][1] = -proj[1][1];
		return proj * view;
	}
}