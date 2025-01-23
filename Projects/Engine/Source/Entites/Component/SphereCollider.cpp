#include "SphereCollider.h"
#include "Enum.h"
#include "DebugRenderer.h"
#include "Transform.h"

namespace Odyssey
{
	SphereCollider::SphereCollider(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	SphereCollider::SphereCollider(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void SphereCollider::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", SphereCollider::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Physics Layer", Enum::ToString(m_PhysicsLayer));
		componentNode.WriteData("Center", m_Center);
		componentNode.WriteData("Radius", m_Radius);
	}

	void SphereCollider::Deserialize(SerializationNode& node)
	{
		std::string layer;
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Physics Layer", layer);
		node.ReadData("Center", m_Center);
		node.ReadData("Radius", m_Radius);

		if (!layer.empty())
			m_PhysicsLayer = Enum::ToEnum<PhysicsLayer>(layer);
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