#include "Transform.h"
#include <Logger.h>
#include <glm.h>
#include <Yaml.h>

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, Transform);

	Transform::Transform(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{
		m_Position = glm::vec3(0, 0, 0);
		m_EulerRotation = glm::vec3(0, 0, 0);
		m_Rotation = glm::quat(1, 0, 0, 0);
		m_Scale = glm::vec3(1, 1, 1);
		m_Dirty = true;
	}

	void Transform::AddPosition(glm::vec3 pos)
	{
		m_Position += pos;
		m_Dirty = true;
	}

	void Transform::AddPosition(float x, float y, float z)
	{
		m_Position.x += x;
		m_Position.y += y;
		m_Position.z += z;
		m_Dirty = true;
	}

	void Transform::SetPosition(glm::vec3 pos)
	{
		m_Position = pos;
		m_Dirty = true;
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		m_Position.x = x;
		m_Position.y = y;
		m_Position.z = z;
		m_Dirty = true;
	}

	void Transform::AddRotation(glm::vec3 eulerAngles)
	{
		m_EulerRotation += eulerAngles;
		m_EulerRotation.x = abs(m_EulerRotation.x) > 360.0f ? m_EulerRotation.x / 360.0f : m_EulerRotation.x;
		m_EulerRotation.y = abs(m_EulerRotation.y) > 360.0f ? m_EulerRotation.y / 360.0f : m_EulerRotation.y;
		m_EulerRotation.z = abs(m_EulerRotation.z) > 360.0f ? m_EulerRotation.z / 360.0f : m_EulerRotation.z;
		m_Rotation = glm::quat(glm::vec3(glm::radians(m_EulerRotation.x), glm::radians(m_EulerRotation.y), glm::radians(m_EulerRotation.z)));
		m_Dirty = true;
	}

	void Transform::AddRotation(glm::quat orientation)
	{
		m_Rotation = orientation * m_Rotation;
		m_Dirty = true;
	}

	void Transform::SetRotation(glm::vec3 eulerAngles)
	{
		m_EulerRotation = eulerAngles;
		m_Rotation = glm::quat(glm::vec3(glm::radians(eulerAngles.x), glm::radians(eulerAngles.y), glm::radians(eulerAngles.z)));
		m_Dirty = true;
	}

	void Transform::SetRotation(glm::quat orientation)
	{
		m_Rotation = orientation;
		m_EulerRotation = glm::degrees(glm::eulerAngles(m_Rotation));
		m_Dirty = true;
	}

	void Transform::AddScale(glm::vec3 scaleFactor)
	{
		m_Scale += scaleFactor;
		m_Dirty = true;
	}

	void Transform::AddScale(float x, float y, float z)
	{
		m_Scale.x += x;
		m_Scale.y += y;
		m_Scale.z += z;
		m_Dirty = true;
	}

	void Transform::SetScale(glm::vec3 scale)
	{
		if (m_Scale != scale)
		{
			m_Scale = scale;
			m_Dirty = true;
		}
	}

	void Transform::SetScale(float x, float y, float z)
	{
		SetScale(glm::vec3(x, y, z));
	}

	void Transform::SetLocalMatrix(glm::mat4 localMatrix)
	{
		m_LocalMatrix = localMatrix;

		glm::vec3 scale;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(localMatrix, scale, m_Rotation, m_Position, skew, perspective);
	}

	glm::vec3 Transform::Forward()
	{
		glm::vec3 row2 = glm::column(m_LocalMatrix, 2);
		return glm::normalize(row2);
	}

	glm::vec3 Transform::Right()
	{
		glm::vec3 row2 = glm::column(m_LocalMatrix, 0);
		return glm::normalize(row2);
	}

	glm::vec3 Transform::Up()
	{
		glm::vec3 row2 = glm::column(m_LocalMatrix, 1);
		return glm::normalize(row2);
	}

	glm::mat4 Transform::GetLocalMatrix()
	{
		ComposeLocalMatrix();
		return m_LocalMatrix;
	}

	glm::mat4x4 Transform::GetWorldMatrix()
	{
		ComposeLocalMatrix();
		glm::mat4 worldMatrix = m_LocalMatrix;

		GameObject parent = m_GameObject.GetParent();
		while (parent)
		{
			Transform& parentTransform = parent.GetComponent<Transform>();
			worldMatrix = parentTransform.GetLocalMatrix() * worldMatrix;

			parent = parent.GetParent();
		}

		return worldMatrix;
	}

	void Transform::RotateAround(glm::vec3 center, glm::vec3 axis, float degrees, bool worldSpace)
	{
		glm::quat targetRotation = glm::angleAxis(glm::radians(degrees), axis);
		glm::vec3 directionToTarget = m_Position - center;
		directionToTarget = targetRotation * directionToTarget;
		SetPosition(center + directionToTarget);

		if (worldSpace)
		{
			// Target * Current = world space rotation
			SetRotation(glm::normalize(targetRotation * m_Rotation));
		}
		else
		{
			// Current * target = local space rotation
			SetRotation(glm::normalize(m_Rotation * targetRotation));
		}
	}

	void Transform::CalculateEulerRotations()
	{
		// Check if the row 3 column 1 is +- 1
		if (m_LocalMatrix[2][0] != 1 && m_LocalMatrix[2][0] != -1)
		{
			// Calculate the euler angle rotation from the world matrix
			float x = -std::asin(m_LocalMatrix[2][0]);
			float cosX = std::cos(x);
			float y = std::atan2(m_LocalMatrix[2][1] / cosX, m_LocalMatrix[2][2] / cosX);
			float z = std::atan2(m_LocalMatrix[1][0] / cosX, m_LocalMatrix[0][0] / cosX);

			// Set the calculated rotation value
			m_EulerRotation = glm::vec3(x, y, z);
		}
		else
		{
			// Calculate the euler angle rotation from the world matrix
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			if (m_LocalMatrix[2][0] == -1)
			{
				x = glm::half_pi<float>();
				y = x + std::atan2(m_LocalMatrix[0][1], m_LocalMatrix[0][2]);
			}
			else
			{
				x = -glm::half_pi<float>();
				y = -x + std::atan2(-m_LocalMatrix[0][1], -m_LocalMatrix[0][2]);
			}

			// Set the calculated rotation value
			m_EulerRotation = glm::vec3(x, y, z);
		}
	}

	void Transform::ComposeLocalMatrix()
	{
		// Compose the position, rotation and scale into 3 matrices
		if (m_Dirty)
		{
			mat4x4 t = glm::translate(glm::identity<mat4x4>(), m_Position);
			mat4x4 r = glm::toMat4(m_Rotation);
			mat4x4 s = glm::scale(glm::identity<mat4x4>(), m_Scale);
			m_LocalMatrix = t * r * s;
			m_Dirty = false;
		}
	}

	void Transform::Reset()
	{
		m_Position = glm::vec3(0, 0, 0);
		m_EulerRotation = glm::vec3(0, 0, 0);
		m_Rotation = glm::quat(0, 0, 0, 1);
		m_Scale = glm::vec3(1, 1, 1);

		//parent = nullptr;
		m_LocalMatrix = glm::identity<glm::mat4>();
		m_WorldMatrix = glm::identity<glm::mat4>();
		ComposeLocalMatrix();

		m_Dirty = true;
	}

	void Transform::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", Transform::Type);
		componentNode.WriteData("Position", m_Position);
		componentNode.WriteData("Rotation", m_EulerRotation);
		componentNode.WriteData("Scale", m_Scale);
	}

	void Transform::Deserialize(SerializationNode& node)
	{
		node.ReadData("Position", m_Position);
		node.ReadData("Rotation", m_EulerRotation);
		node.ReadData("Scale", m_Scale);

		// TODO: This is a hack to set the quat rotation on deserialization
		// Just serialize out the quat instead
		SetRotation(m_EulerRotation);
		ComposeLocalMatrix();
	}
}