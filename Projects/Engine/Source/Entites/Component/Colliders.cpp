#include "Colliders.h"
#include "Transform.h"
#include "DebugRenderer.h"

namespace Odyssey
{
	BoxCollider::BoxCollider(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	BoxCollider::BoxCollider(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void BoxCollider::OnDestroy()
	{
		SetDebugEnabled(false);
	}

	void BoxCollider::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", BoxCollider::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Center", m_Center);
		componentNode.WriteData("Extents", m_Extents);
	}

	void BoxCollider::Deserialize(SerializationNode& node)
	{
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Center", m_Center);
		node.ReadData("Extents", m_Extents);
	}

	void BoxCollider::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}

	void BoxCollider::SetDebugEnabled(bool enabled)
	{
		if (enabled != m_DebugEnabled)
		{
			m_DebugEnabled = enabled;

			if (m_DebugEnabled)
				m_DebugID = DebugRenderer::Register([this]() { DebugDraw(); });
			else
				DebugRenderer::Deregister(m_DebugID);
		}
	}
	void BoxCollider::DebugDraw()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			float3 position, scale;
			quat rotation;
			transform->DecomposeWorldMatrix(position, rotation, scale);

			float3 right = transform->Right();
			float3 up = transform->Up();
			float3 forward = transform->Forward();

			DebugRenderer::AddOrientedBox(position + m_Center, m_Extents, right, up, forward, float3(0.0f, 1.0f, 0.0f));
		}
	}

	CapsuleCollider::CapsuleCollider(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	CapsuleCollider::CapsuleCollider(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void CapsuleCollider::OnDestroy()
	{
		SetDebugEnabled(false);
	}

	void CapsuleCollider::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", CapsuleCollider::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Center", m_Center);
		componentNode.WriteData("Radius", m_Radius);
		componentNode.WriteData("Height", m_Height);
	}

	void CapsuleCollider::Deserialize(SerializationNode& node)
	{
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Center", m_Center);
		node.ReadData("Radius", m_Radius);
		node.ReadData("Height", m_Height);
	}

	void CapsuleCollider::SetDebugEnabled(bool enabled)
	{
		if (enabled != m_DebugEnabled)
		{
			m_DebugEnabled = enabled;

			if (m_DebugEnabled)
				m_DebugID = DebugRenderer::Register([this]() { DebugDraw(); });
			else
				DebugRenderer::Deregister(m_DebugID);
		}
	}

	void CapsuleCollider::DebugDraw()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
			DebugRenderer::AddCapsule(transform->GetWorldPosition() + m_Center, m_Radius, m_Height * 0.5f, float3(0.0f, 1.0f, 0.0f));
	}

	SphereCollider::SphereCollider(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	SphereCollider::SphereCollider(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void SphereCollider::OnDestroy()
	{
		SetDebugEnabled(false);
	}

	void SphereCollider::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", SphereCollider::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Center", m_Center);
		componentNode.WriteData("Radius", m_Radius);
	}

	void SphereCollider::Deserialize(SerializationNode& node)
	{
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Center", m_Center);
		node.ReadData("Radius", m_Radius);
	}

	void SphereCollider::SetDebugEnabled(bool enabled)
	{
		if (enabled != m_DebugEnabled)
		{
			m_DebugEnabled = enabled;

			if (m_DebugEnabled)
				m_DebugID = DebugRenderer::Register([this]() { DebugDraw(); });
			else
				DebugRenderer::Deregister(m_DebugID);
		}
	}

	void SphereCollider::DebugDraw()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
			DebugRenderer::AddSphere(transform->GetWorldPosition() + m_Center, m_Radius, float3(0.0f, 1.0f, 0.0f));
	}
}