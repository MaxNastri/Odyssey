#include "Transform.h"
#include <Log.h>
#include <glm.h>
#include <Yaml.h>

namespace Odyssey
{
	Transform::Transform(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{
		Reset();
	}

	Transform::Transform(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Reset();
		Deserialize(node);
	}

	void Transform::SetPosition(float3 position)
	{
		m_Position = position;
		m_Dirty = true;
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		m_Position.x = x;
		m_Position.y = y;
		m_Position.z = z;
		m_Dirty = true;
	}

	void Transform::SetRotation(float3 eulerAngles)
	{
		m_EulerRotation = eulerAngles;
		m_Rotation = quat(float3(glm::radians(eulerAngles.x), glm::radians(eulerAngles.y), glm::radians(eulerAngles.z)));
		m_Dirty = true;
	}

	void Transform::SetRotation(quat orientation)
	{
		m_Rotation = orientation;
		m_EulerRotation = glm::degrees(glm::eulerAngles(m_Rotation));
		m_Dirty = true;
	}

	void Transform::SetScale(float3 scale)
	{
		if (m_Scale != scale)
		{
			m_Scale = scale;
			m_Dirty = true;
		}
	}

	void Transform::SetScale(float x, float y, float z)
	{
		SetScale(float3(x, y, z));
	}

	void Transform::SetLocalMatrix(mat4 localMatrix)
	{
		m_LocalMatrix = localMatrix;

		float3 scale;
		float3 skew;
		float4 perspective;
		glm::decompose(localMatrix, scale, m_Rotation, m_Position, skew, perspective);
	}

	float3 Transform::GetWorldPosition()
	{
		mat4 worldMatrix = GetWorldMatrix();
		float3 position = glm::column(worldMatrix, 3);
		return position;
	}

	float3 Transform::Forward()
	{
		mat4 worldMatrix = GetWorldMatrix();
		float3 row2 = glm::column(worldMatrix, 2);
		return glm::normalize(row2);
	}

	float3 Transform::Right()
	{
		mat4 worldMatrix = GetWorldMatrix();
		float3 row2 = glm::column(worldMatrix, 0);
		return glm::normalize(row2);
	}

	float3 Transform::Up()
	{
		mat4 worldMatrix = GetWorldMatrix();
		float3 row2 = glm::column(worldMatrix, 1);
		return glm::normalize(row2);
	}

	mat4 Transform::GetLocalMatrix()
	{
		ComposeLocalMatrix();
		return m_LocalMatrix;
	}

	mat4 Transform::GetWorldMatrix()
	{
		ComposeLocalMatrix();
		mat4 worldMatrix = m_LocalMatrix;

		GameObject parent = m_GameObject.GetParent();
		while (parent.IsValid())
		{
			if (Transform* parentTransform = parent.TryGetComponent<Transform>())
			{
				worldMatrix = parentTransform->GetLocalMatrix() * worldMatrix;
				parent = parent.GetParent();
			}
			else
				break;
		}

		return worldMatrix;
	}

	void Transform::SetLocalSpace()
	{
		GameObject parent = m_GameObject.GetParent();
		if (parent.IsValid())
		{
			if (Transform* parentTransform = parent.TryGetComponent<Transform>())
			{
				ComposeLocalMatrix();
				m_LocalMatrix = glm::inverse(parentTransform->GetWorldMatrix()) * m_LocalMatrix;
			}
		}

	}

	void Transform::DecomposeWorldMatrix(float3& position, quat& rotation, float3& scale)
	{
		float3 skew;
		float4 perspective;
		glm::decompose(GetWorldMatrix(), scale, rotation, position, skew, perspective);
	}

	void Transform::RotateAround(float3 center, float3 axis, float degrees, bool worldSpace)
	{
		quat targetRotation = glm::angleAxis(glm::radians(degrees), axis);
		float3 directionToTarget = m_Position - center;
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

	void Transform::LookAt(float3 center, float3 up)
	{
		SetLocalMatrix(glm::inverse(glm::lookAtLH(m_Position, center, up)));
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
			m_EulerRotation = float3(x, y, z);
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
			m_EulerRotation = float3(x, y, z);
		}
	}

	void Transform::ComposeLocalMatrix()
	{
		// Compose the position, rotation and scale into 3 matrices
		if (m_Dirty)
		{
			mat4 t = glm::translate(mat4(1.0f), m_Position);
			mat4 r = glm::toMat4(m_Rotation);
			mat4 s = glm::scale(mat4(1.0f), m_Scale);
			m_LocalMatrix = t * r * s;
			m_Dirty = false;
		}
	}

	void Transform::Reset()
	{
		m_Position = float3(0, 0, 0);
		m_EulerRotation = float3(0, 0, 0);
		m_Rotation = quat(0, 0, 0, 1);
		m_Scale = float3(1, 1, 1);

		//parent = nullptr;
		m_LocalMatrix = mat4(1.0f);
		m_WorldMatrix = mat4(1.0f);
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