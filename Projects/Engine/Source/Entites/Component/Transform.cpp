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

	void Transform::SetRotation(float3 eulerAngles, bool radians)
	{
		// Assign the euler angles rotation and wrap the rotation between [-pi, pi]
		m_EulerRotation = radians ? eulerAngles : glm::radians(eulerAngles);
		m_EulerRotation = glm::mod(m_EulerRotation + glm::pi<float>(), 2.0f * glm::pi<float>()) - glm::pi<float>();

		// Assign the quaternion rotation
		m_Rotation = quat(m_EulerRotation);
		m_Dirty = true;
	}

	void Transform::SetRotation(quat orientation)
	{
		m_Rotation = orientation;
		SetEulerRotation(glm::eulerAngles(m_Rotation));
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

	float3 Transform::GetEulerRotation(bool radians)
	{
		return radians ? m_EulerRotation : glm::degrees(m_EulerRotation);
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

				float3 skew;
				float4 perspective;
				glm::decompose(m_LocalMatrix, m_Scale, m_Rotation, m_Position, skew, perspective);
				m_EulerRotation = glm::eulerAngles(m_Rotation);
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

	void Transform::SetEulerRotation(float3 eulerAngles)
	{
		// wrap given euler angles to range [-pi, pi]
		auto wrapToPi = [](glm::vec3 v)
			{
				return glm::mod(v + glm::pi<float>(), 2.0f * glm::pi<float>()) - glm::pi<float>();
			};

		float3 originalEuler = m_EulerRotation;
		m_EulerRotation = eulerAngles;

		// A given quat can be represented by many Euler angles (technically infinitely many),
		// and glm::eulerAngles() can only give us one of them which may or may not be the one we want.
		// Here we have a look at some likely alternatives and pick the one that is closest to the original Euler angles.
		// This is an attempt to avoid sudden 180deg flips in the Euler angles when we SetRotation(quat).

		glm::vec3 alternate1 = { m_EulerRotation.x - glm::pi<float>(), glm::pi<float>() - m_EulerRotation.y, m_EulerRotation.z - glm::pi<float>() };
		glm::vec3 alternate2 = { m_EulerRotation.x + glm::pi<float>(), glm::pi<float>() - m_EulerRotation.y, m_EulerRotation.z - glm::pi<float>() };
		glm::vec3 alternate3 = { m_EulerRotation.x + glm::pi<float>(), glm::pi<float>() - m_EulerRotation.y, m_EulerRotation.z + glm::pi<float>() };
		glm::vec3 alternate4 = { m_EulerRotation.x - glm::pi<float>(), glm::pi<float>() - m_EulerRotation.y, m_EulerRotation.z + glm::pi<float>() };

		// We pick the alternative that is closest to the original value.
		float distance0 = glm::length2(wrapToPi(m_EulerRotation - originalEuler));
		float distance1 = glm::length2(wrapToPi(alternate1 - originalEuler));
		float distance2 = glm::length2(wrapToPi(alternate2 - originalEuler));
		float distance3 = glm::length2(wrapToPi(alternate3 - originalEuler));
		float distance4 = glm::length2(wrapToPi(alternate4 - originalEuler));

		float best = distance0;
		if (distance1 < best)
		{
			best = distance1;
			m_EulerRotation = alternate1;
		}
		if (distance2 < best)
		{
			best = distance2;
			m_EulerRotation = alternate2;
		}
		if (distance3 < best)
		{
			best = distance3;
			m_EulerRotation = alternate3;
		}
		if (distance4 < best)
		{
			best = distance4;
			m_EulerRotation = alternate4;
		}

		m_EulerRotation = wrapToPi(m_EulerRotation);
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
		m_Rotation = quat(1,0,0,0);
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
		componentNode.WriteData("Rotation", m_Rotation);
		componentNode.WriteData("Scale", m_Scale);
	}

	void Transform::Deserialize(SerializationNode& node)
	{
		node.ReadData("Position", m_Position);
		node.ReadData("Rotation", m_Rotation);
		node.ReadData("Scale", m_Scale);

		m_EulerRotation = glm::eulerAngles(m_Rotation);
		ComposeLocalMatrix();
	}
}